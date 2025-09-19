# Healthy

A comprehensive health check tool for the Helipad cross-platform game engine project. Healthy verifies your development environment, dependencies, and configuration to ensure a smooth setup and build process.

## Description

Healthy is a colorful, interactive command-line tool that systematically validates your development environment for the Helipad game engine. It performs sequential checks of critical dependencies, provides clear feedback on issues, and offers actionable solutions when problems are detected.

The tool uses a checklist-style interface with progress bars, colored output, and detailed diagnostics to guide developers through environment setup and troubleshooting.

## Features

- **Cross-Platform Support**: Works seamlessly on Windows, macOS, and Linux
- **Platform Detection**: Identifies your operating system and architecture
- **Dependency Validation**: Checks for required tools (Git, CMake, C++20 compiler)
- **Optional Tools**: Validates optional but recommended tools (Doxygen, ccache)
- **Submodule Verification**: Ensures Git submodules are properly initialized and up-to-date
- **Build Environment**: Validates CMake configuration and C++20 compiler compatibility
- **Interactive Feedback**: Colorful progress indicators and clear error messaging
- **Guided Troubleshooting**: Provides platform-specific installation instructions for missing dependencies

## CLI Usage Examples

```bash
# Basic health check (default behavior)
uv run healthy

# Verbose output with detailed diagnostics
uv run healthy --verbose

# Check specific components only
uv run healthy --check git cmake compiler

# Include optional checks
uv run healthy --include-optional doxygen ccache

# Skip optional checks (default behavior)
uv run healthy --skip-optional

# Platform-specific installation guidance
uv run healthy --install-help git

# Skip interactive prompts (for CI/automation)
uv run healthy --non-interactive

# Generate health report file
uv run healthy --report health-report.json

# Show version and available checks
uv run healthy --version
uv run healthy --list-checks
```

## Project Plan

### Phase 1: Core Infrastructure

- [ ] Set up CLI framework (Typer + Rich)
- [ ] Implement base health check system
- [ ] Create progress tracking and colorful output
- [ ] Design check result reporting system

### Phase 2: System Checks

- [ ] Platform and architecture detection (Windows/macOS/Linux, x64/ARM64/x86)
- [ ] Git installation and version verification
- [ ] CMake installation and version verification  
- [ ] C++20 compiler detection via CMake (MSVC/GCC/Clang)
- [ ] Cross-platform path and executable handling

### Phase 3: Optional Tool Checks

- [ ] Doxygen detection for documentation generation
- [ ] ccache detection for build acceleration
- [ ] Platform-specific tool installation guidance
- [ ] Configurable optional check selection

### Phase 4: Project-Specific Checks

- [ ] Git repository validation
- [ ] Submodule initialization checks
- [ ] Submodule update verification
- [ ] Build directory structure validation

### Phase 5: Advanced Features

- [ ] Interactive troubleshooting prompts
- [ ] Automated fix suggestions
- [ ] Configuration file generation
- [ ] CI/CD integration support

### Phase 6: Cross-Platform Polish

- [ ] Platform-specific error messages and installation guides
- [ ] Cross-platform testing (Windows, macOS, Linux)
- [ ] Package manager integration (winget, brew, apt, pacman)
- [ ] Performance optimization
- [ ] Comprehensive documentation

## Implementation Plan

### Architecture Overview

```text
healthy/
├── main.py              # CLI entry point and command definitions
├── core/
│   ├── __init__.py
│   ├── checker.py       # Base health check framework
│   ├── reporter.py      # Progress and result reporting
│   └── config.py        # Configuration management
├── checks/
│   ├── __init__.py
│   ├── system.py        # Platform/architecture checks
│   ├── tools.py         # Git, CMake, compiler checks
│   ├── optional.py      # Doxygen, ccache, and other optional tools
│   ├── project.py       # Helipad-specific checks
│   └── submodules.py    # Git submodule validation
└── utils/
    ├── __init__.py
    ├── platform.py      # Cross-platform detection utilities
    ├── process.py       # Cross-platform subprocess execution
    ├── filesystem.py    # Cross-platform file/directory utilities
    └── installers.py    # Platform-specific installation guides
```

### Check Sequence

1. **System Environment**
   - Detect OS (Windows, macOS, Linux) and distribution
   - Identify architecture (x64, ARM64, x86)
   - Verify Python version compatibility
   - Check platform-specific requirements

2. **Core Dependencies**
   - Git: Version ≥ 2.20 for submodule support
   - CMake: Version ≥ 3.20 for C++20 features
   - C++20 Compiler: Test compilation via CMake
     - Windows: MSVC 2019+, MinGW-w64, or Clang
     - macOS: Xcode Command Line Tools or Homebrew Clang
     - Linux: GCC 10+ or Clang 10+

3. **Optional Tools** (configurable)
   - Doxygen: Documentation generation (any recent version)
   - ccache: Build acceleration (version ≥ 3.7)
   - Platform-specific recommendations

4. **Project Structure**
   - Validate in Helipad repository
   - Check external/ directory structure
   - Verify CMakeLists.txt presence
   - Cross-platform path validation

5. **Submodule Status**
   - List expected submodules (SDL, glm, entt, etc.)
   - Check initialization status
   - Verify checkout integrity
   - Detect outdated submodules

6. **Build Readiness**
   - Test CMake configuration on current platform
   - Validate build directory setup
   - Check for platform-specific build issues
   - Verify compiler toolchain compatibility

### Error Handling Strategy

- **Cross-Platform Compatibility**: Handle platform-specific paths, executables, and commands
- **Graceful Degradation**: Continue checking when non-critical issues found
- **Platform-Aware Messaging**: OS-specific error descriptions and installation instructions
- **Actionable Solutions**: Direct commands/package manager instructions for each platform
  - Windows: winget, chocolatey, or direct download links
  - macOS: Homebrew, MacPorts, or App Store links
  - Linux: apt, yum, pacman, or build-from-source instructions
- **Exit Codes**: Meaningful return codes for automation and CI/CD integration

### User Experience Design

- **Progress Visualization**: Rich progress bars for long-running checks
- **Color Coding**: Green (✓), Red (✗), Yellow (⚠) status indicators
- **Structured Output**: Organized sections with clear headers
- **Summary Report**: Final status with action items

### Dependencies

- **Typer**: Modern CLI framework with auto-help generation
- **Rich**: Beautiful terminal formatting, progress bars, panels
- **Standard Library**: subprocess, platform, pathlib for core functionality

## Development Status

Currently in planning phase. See project plan above for implementation roadmap.

## Contributing

This tool is part of the Helipad game engine project. Follow the project's contribution guidelines and coding standards when implementing features.
