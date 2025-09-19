"""
Development tool health checks (Git, CMake, compilers).
"""

import subprocess
import tempfile
import os
from typing import List

from core.checker import HealthCheck
from core.types import CheckResult, CheckStatus
from core.config import CheckConfig


class GitCheck(HealthCheck):
    """Check Git installation and version."""
    
    def __init__(self):
        super().__init__(
            name="git",
            description="Verify Git installation and version"
        )
    
    async def run(self, config: CheckConfig) -> CheckResult:
        """Run the Git check."""
        try:
            result = subprocess.run(
                ["git", "--version"], 
                capture_output=True, 
                text=True, 
                check=True
            )
            
            version_line = result.stdout.strip()
            # Extract version number (e.g., "git version 2.34.1")
            if "git version" in version_line:
                version_str = version_line.split()[-1]
                version_parts = version_str.split(".")
                if len(version_parts) >= 2:
                    major, minor = int(version_parts[0]), int(version_parts[1])
                    if major > 2 or (major == 2 and minor >= 20):
                        return CheckResult(
                            status=CheckStatus.PASSED,
                            message=f"Git {version_str}",
                            details=version_line
                        )
                    else:
                        return CheckResult(
                            status=CheckStatus.WARNING,
                            message=f"Git {version_str} is old",
                            suggestion="Consider upgrading to Git 2.20+ for better submodule support"
                        )
            
            return CheckResult(
                status=CheckStatus.PASSED,
                message="Git is installed",
                details=version_line
            )
            
        except (subprocess.CalledProcessError, FileNotFoundError):
            return CheckResult(
                status=CheckStatus.FAILED,
                message="Git is not installed or not in PATH",
                suggestion="Please install Git from https://git-scm.com/"
            )
        except Exception as e:
            return CheckResult(
                status=CheckStatus.FAILED,
                message="Failed to check Git",
                error=e
            )


class CMakeCheck(HealthCheck):
    """Check CMake installation and version."""
    
    def __init__(self):
        super().__init__(
            name="cmake",
            description="Verify CMake installation and version"
        )
    
    async def run(self, config: CheckConfig) -> CheckResult:
        """Run the CMake check."""
        try:
            result = subprocess.run(
                ["cmake", "--version"], 
                capture_output=True, 
                text=True, 
                check=True
            )
            
            version_line = result.stdout.strip().split('\n')[0]
            # Extract version (e.g., "cmake version 3.22.1")
            if "cmake version" in version_line:
                version_str = version_line.split()[-1]
                version_parts = version_str.split(".")
                if len(version_parts) >= 2:
                    major, minor = int(version_parts[0]), int(version_parts[1])
                    if major > 3 or (major == 3 and minor >= 20):
                        return CheckResult(
                            status=CheckStatus.PASSED,
                            message=f"CMake {version_str}",
                            details=version_line
                        )
                    else:
                        return CheckResult(
                            status=CheckStatus.FAILED,
                            message=f"CMake {version_str} is too old",
                            suggestion="Please upgrade to CMake 3.20+ for C++20 support"
                        )
            
            return CheckResult(
                status=CheckStatus.PASSED,
                message="CMake is installed",
                details=version_line
            )
            
        except (subprocess.CalledProcessError, FileNotFoundError):
            return CheckResult(
                status=CheckStatus.FAILED,
                message="CMake is not installed or not in PATH",
                suggestion="Please install CMake from https://cmake.org/download/"
            )
        except Exception as e:
            return CheckResult(
                status=CheckStatus.FAILED,
                message="Failed to check CMake",
                error=e
            )


class CppCompilerCheck(HealthCheck):
    """Check C++20 compatible compiler via CMake."""
    
    def __init__(self):
        super().__init__(
            name="cpp_compiler", 
            description="Test C++20 compiler compatibility via CMake"
        )
        self.add_dependency("cmake")  # Requires CMake to be available
    
    async def run(self, config: CheckConfig) -> CheckResult:
        """Run the C++ compiler check."""
        try:
            with tempfile.TemporaryDirectory() as temp_dir:
                # Write CMakeLists.txt
                cmake_content = """
cmake_minimum_required(VERSION 3.20)
project(TestCpp20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(test_cpp20 main.cpp)
"""
                with open(os.path.join(temp_dir, "CMakeLists.txt"), "w") as f:
                    f.write(cmake_content)
                
                # Write main.cpp
                cpp_content = """
#include <iostream>
int main() {
    std::cout << "C++20 test" << std::endl;
    return 0;
}
"""
                with open(os.path.join(temp_dir, "main.cpp"), "w") as f:
                    f.write(cpp_content)
                
                # Configure with CMake
                config_result = subprocess.run(
                    ["cmake", "."], 
                    cwd=temp_dir, 
                    capture_output=True, 
                    text=True, 
                    check=True
                )
                
                # Build
                build_result = subprocess.run(
                    ["cmake", "--build", "."], 
                    cwd=temp_dir, 
                    capture_output=True, 
                    text=True, 
                    check=True
                )
                
                return CheckResult(
                    status=CheckStatus.PASSED,
                    message="C++20 compatible compiler detected",
                    details="Successfully compiled C++20 test program via CMake"
                )
                
        except subprocess.CalledProcessError as e:
            return CheckResult(
                status=CheckStatus.FAILED,
                message="No C++20 compatible compiler found",
                suggestion="Please install a C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)",
                details=f"CMake error: {e.stderr if e.stderr else str(e)}"
            )
        except Exception as e:
            return CheckResult(
                status=CheckStatus.FAILED,
                message="Failed to test C++ compiler",
                error=e
            )


class ToolChecks:
    """Collection of development tool health checks."""
    
    @staticmethod
    def get_checks() -> List[HealthCheck]:
        """Get all tool checks."""
        return [
            GitCheck(),
            CMakeCheck(), 
            CppCompilerCheck(),
        ]
