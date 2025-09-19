"""
Platform-specific installation instructions.
"""

import platform
from typing import Optional


def get_install_instructions(tool: str) -> Optional[str]:
    """Get platform-specific installation instructions for a tool."""
    system = platform.system().lower()
    
    instructions = {
        "git": {
            "windows": """
# Installing Git on Windows

## Option 1: Official Installer
Download from https://git-scm.com/download/win

## Option 2: Package Managers
- **winget**: `winget install Git.Git`
- **chocolatey**: `choco install git`
- **scoop**: `scoop install git`

> **Note**: After installation, restart your terminal.
""",
            "darwin": """
# Installing Git on macOS

## Option 1: Xcode Command Line Tools (Recommended)
```bash
xcode-select --install
```

## Option 2: Homebrew
```bash
brew install git
```

## Option 3: MacPorts
```bash
sudo port install git
```

## Option 4: Official Installer
Download from https://git-scm.com/download/mac
""",
            "linux": """
# Installing Git on Linux

## Ubuntu/Debian
```bash
sudo apt update && sudo apt install git
```

## RHEL/CentOS/Fedora
- **RHEL/CentOS**: `sudo yum install git` or `sudo dnf install git`
- **Fedora**: `sudo dnf install git`

## Arch Linux
```bash
sudo pacman -S git
```

## SUSE
```bash
sudo zypper install git
```
"""
        },
        
        "cmake": {
            "windows": """
# Installing CMake on Windows

## Option 1: Official Installer (Recommended)
Download from https://cmake.org/download/
Choose "Windows x64 Installer" and add CMake to PATH

## Option 2: Package Managers
- **winget**: `winget install Kitware.CMake`
- **chocolatey**: `choco install cmake`
- **scoop**: `scoop install cmake`

> **Note**: After installation, restart your terminal.
""",
            "darwin": """
# Installing CMake on macOS

## Option 1: Homebrew (Recommended)
```bash
brew install cmake
```

## Option 2: MacPorts
```bash
sudo port install cmake
```

## Option 3: Official Installer
Download from https://cmake.org/download/
Choose "macOS 10.13 or later"

## Option 4: Xcode Command Line Tools
CMake may be included with Xcode tools
""",
            "linux": """
# Installing CMake on Linux

## Ubuntu/Debian
```bash
sudo apt update && sudo apt install cmake
```

## RHEL/CentOS/Fedora
- **RHEL/CentOS**: `sudo yum install cmake` or `sudo dnf install cmake`
- **Fedora**: `sudo dnf install cmake`

## Arch Linux
```bash
sudo pacman -S cmake
```

## SUSE
```bash
sudo zypper install cmake
```

## From Source (for latest version)
Download from https://cmake.org/download/
"""
        },
        
        "doxygen": {
            "windows": """
# Installing Doxygen on Windows

## Option 1: Official Installer
Download from https://www.doxygen.nl/download.html

## Option 2: Package Managers
- **chocolatey**: `choco install doxygen.install`
- **winget**: `winget install DimitriVanHeesch.Doxygen`

> **Note**: After installation, restart your terminal.
""",
            "darwin": """
# Installing Doxygen on macOS

## Option 1: Homebrew (Recommended)
```bash
brew install doxygen
```

## Option 2: MacPorts
```bash
sudo port install doxygen
```

## Option 3: Official Binary
Download from https://www.doxygen.nl/download.html
""",
            "linux": """
# Installing Doxygen on Linux

## Ubuntu/Debian
```bash
sudo apt update && sudo apt install doxygen
```

## RHEL/CentOS/Fedora
- **RHEL/CentOS**: `sudo yum install doxygen` or `sudo dnf install doxygen`
- **Fedora**: `sudo dnf install doxygen`

## Arch Linux
```bash
sudo pacman -S doxygen
```

## SUSE
```bash
sudo zypper install doxygen
```
"""
        },
        
        "ccache": {
            "windows": """
# Installing ccache on Windows

## Option 1: Package Managers
- **chocolatey**: `choco install ccache`
- **vcpkg**: `vcpkg install ccache`

## Option 2: Pre-compiled Binaries
Download from https://github.com/ccache/ccache/releases

## Option 3: Build from Source
Follow instructions at https://ccache.dev/

> **Note**: ccache support on Windows is experimental.
""",
            "darwin": """
# Installing ccache on macOS

## Option 1: Homebrew (Recommended)
```bash
brew install ccache
```

## Option 2: MacPorts
```bash
sudo port install ccache
```

## Option 3: Build from Source
Download from https://ccache.dev/
""",
            "linux": """
# Installing ccache on Linux

## Ubuntu/Debian
```bash
sudo apt update && sudo apt install ccache
```

## RHEL/CentOS/Fedora
- **RHEL/CentOS**: `sudo yum install ccache` or `sudo dnf install ccache`
- **Fedora**: `sudo dnf install ccache`

## Arch Linux
```bash
sudo pacman -S ccache
```

## SUSE
```bash
sudo zypper install ccache
```
"""
        }
    }
    
    tool_lower = tool.lower()
    if tool_lower not in instructions:
        return None
    
    platform_instructions = instructions[tool_lower]
    if system in platform_instructions:
        return platform_instructions[system].strip()
    
    # Fallback to generic instructions if available
    return None
    