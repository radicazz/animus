"""
System-level health checks (platform, architecture, Python version).
"""

import platform
from typing import List

from core.checker import HealthCheck
from core.types import CheckResult, CheckStatus
from core.config import CheckConfig


class PlatformCheck(HealthCheck):
    """Check the current platform and architecture."""
    
    def __init__(self):
        super().__init__(
            name="platform",
            description="Detect operating system and architecture"
        )
    
    async def run(self, config: CheckConfig) -> CheckResult:
        """Run the platform detection check."""
        try:
            system = platform.system()
            machine = platform.machine()
            release = platform.release()
            
            if system in ["Windows", "Darwin", "Linux"]:
                message = f"{system} {machine}"
                details = f"OS: {system} {release}\nArchitecture: {machine}"
                return CheckResult(
                    status=CheckStatus.PASSED,
                    message=message,
                    details=details
                )
            else:
                return CheckResult(
                    status=CheckStatus.WARNING,
                    message=f"Unsupported platform: {system}",
                    suggestion="This platform may not be fully supported"
                )
        except Exception as e:
            return CheckResult(
                status=CheckStatus.FAILED,
                message="Failed to detect platform",
                error=e
            )


class PythonVersionCheck(HealthCheck):
    """Check Python version compatibility."""
    
    def __init__(self):
        super().__init__(
            name="python_version",
            description="Verify Python version compatibility"
        )
    
    async def run(self, config: CheckConfig) -> CheckResult:
        """Run the Python version check."""
        try:
            version = platform.python_version_tuple()
            major, minor = int(version[0]), int(version[1])
            
            if major >= 3 and minor >= 8:
                return CheckResult(
                    status=CheckStatus.PASSED,
                    message=f"Python {'.'.join(version)}"
                )
            else:
                return CheckResult(
                    status=CheckStatus.FAILED,
                    message=f"Python {'.'.join(version)} is too old",
                    suggestion="Please upgrade to Python 3.8 or newer"
                )
        except Exception as e:
            return CheckResult(
                status=CheckStatus.FAILED,
                message="Failed to check Python version",
                error=e
            )


class SystemChecks:
    """Collection of system-level health checks."""
    
    @staticmethod
    def get_checks() -> List[HealthCheck]:
        """Get all system checks."""
        return [
            PlatformCheck(),
            PythonVersionCheck(),
        ]
