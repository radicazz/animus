# Healthy

A comprehensive health check tool for the Helipad cross-platform game engine project. Healthy verifies your development environment, dependencies, and configuration to ensure a smooth setup and build process.

## Quick Start

### Prerequisites

- **uv**: Modern Python package manager ([installation guide](https://docs.astral.sh/uv/getting-started/installation/))
- **Python 3.13+**: Required by the project

### Setup (from Helipad root directory)

```bash
# Run health checks using the convenient wrapper scripts
tools/healthy           # Linux/macOS  
tools\healthy.cmd       # Windows

# Or directly using uv
uv sync --directory tools/healthy-cli
```

### Direct Usage (from tools/healthy-cli directory)

```bash
cd tools/healthy-cli
uv run healthy check
```

## Description

Healthy is a colorful, interactive command-line tool that systematically validates your development environment for the Helipad game engine. It performs sequential checks of critical dependencies, provides clear feedback on issues, and offers actionable solutions when problems are detected.

The tool uses a checklist-style interface with progress bars, colored output, and detailed diagnostics to guide developers through environment setup and troubleshooting.

## Features

- **✅ Cross-Platform Support**: Works seamlessly on Windows, macOS, and Linux
- **✅ Platform Detection**: Identifies your operating system and architecture
- **✅ Core Dependency Validation**: Checks for required tools (Git, CMake, C++20 compiler)
- **✅ Interactive CLI**: Modern Typer-based interface with Rich styling
- **✅ Progress Tracking**: Animated progress bars and real-time status updates
- **✅ Detailed Reporting**: Color-coded results with timing and diagnostic information
- **✅ Installation Guidance**: Platform-specific installation instructions for missing tools
- **🔄 Optional Tools**: Validates optional but recommended tools (Doxygen, ccache) - *In Progress*
- **🔄 Submodule Verification**: Ensures Git submodules are properly initialized and up-to-date - *Planned*
- **🔄 Build Environment**: Advanced CMake configuration validation - *Planned*
- **🔄 Guided Troubleshooting**: Interactive problem-solving assistance - *Planned*

## CLI Usage Examples

### From Helipad Root Directory

```bash
# Basic health check (runs all required checks)
tools/healthy                                    # Linux/macOS
tools\healthy.cmd                                # Windows

# Verbose output with detailed diagnostics
tools/healthy check --verbose                   # Linux/macOS
tools\healthy.cmd check --verbose               # Windows

# Check specific components only
tools/healthy check --check git --check cmake   # Linux/macOS
tools\healthy.cmd check --check git --check cmake # Windows

# Get installation help for tools
tools/healthy install-help git                  # Linux/macOS
tools\healthy.cmd install-help git              # Windows

# List all available checks
tools/healthy list-checks                       # Linux/macOS
tools\healthy.cmd list-checks                   # Windows

# Show version information
tools/healthy version                           # Linux/macOS
tools\healthy.cmd version                       # Windows
```

### From tools/healthy-cli Directory

```bash
# Basic health check (runs all required checks)
uv run healthy check

# Verbose output with detailed diagnostics and timing
uv run healthy check --verbose

# Check specific components only
uv run healthy check --check git --check cmake

# Platform-specific installation guidance
uv run healthy install-help git
uv run healthy install-help cmake

# Skip interactive prompts (for CI/automation)
uv run healthy check --non-interactive

# List all available checks
uv run healthy list-checks

# Show version information
uv run healthy version

# Include optional checks (when implemented)
uv run healthy check --include-optional doxygen --include-optional ccache

# Generate health report file (planned)
uv run healthy check --report health-report.json
```

## Project Plan

### ✅ Phase 1: Core Infrastructure - COMPLETED

- [x] Set up CLI framework (Typer + Rich)
- [x] Implement base health check system with async support
- [x] Create progress tracking and colorful output
- [x] Design check result reporting system
- [x] Basic system checks (platform, Python version)
- [x] Core tool checks (Git, CMake, C++20 compiler)
- [x] Platform-specific installation guidance system

### 🔄 Phase 2: Enhanced System Checks - IN PROGRESS

- [x] Platform and architecture detection (Windows/macOS/Linux, x64/ARM64/x86)
- [x] Git installation and version verification
- [x] CMake installation and version verification  
- [x] C++20 compiler detection via CMake (MSVC/GCC/Clang)
- [ ] Cross-platform path and executable handling improvements
- [ ] Enhanced version parsing and compatibility checking

### Phase 3: Optional Tool Checks

- [ ] Doxygen detection for documentation generation
- [ ] ccache detection for build acceleration
- [ ] Platform-specific tool installation guidance
- [ ] Configurable optional check selection
- [ ] Build tool ecosystem validation (Ninja, Make, etc.)

### Phase 4: Project-Specific Checks

- [ ] Git repository validation (ensure we're in Helipad repo)
- [ ] Submodule initialization checks
- [ ] Submodule update verification
- [ ] Build directory structure validation
- [ ] CMakeLists.txt content validation

### Phase 5: Advanced Features

- [ ] Interactive troubleshooting prompts
- [ ] Automated fix suggestions with user confirmation
- [ ] Configuration file generation (.healthy.json)
- [ ] CI/CD integration support with machine-readable output
- [ ] Health report generation (JSON, HTML, text formats)

### Phase 6: Cross-Platform Polish

- [ ] Platform-specific error messages and installation guides
- [ ] Cross-platform testing automation (Windows, macOS, Linux)
- [ ] Package manager integration (winget, brew, apt, pacman)
- [ ] Performance optimization for large projects
- [ ] Comprehensive documentation and examples

## Implementation Plan

### Architecture Overview

```text
healthy/
├── main.py              # ✅ CLI entry point and command definitions
├── core/
│   ├── __init__.py      # ✅ Core module initialization
│   ├── checker.py       # ✅ Base health check framework with async support
│   ├── reporter.py      # ✅ Rich-based progress and result reporting
│   ├── config.py        # ✅ Configuration management with env var support
│   └── types.py         # ✅ Shared data structures (CheckResult, CheckStatus)
├── checks/
│   ├── __init__.py      # ✅ Check modules initialization
│   ├── system.py        # ✅ Platform/architecture checks (implemented)
│   ├── tools.py         # ✅ Git, CMake, compiler checks (implemented)
│   ├── optional.py      # 🔄 Doxygen, ccache optional tools (planned)
│   ├── project.py       # 🔄 Helipad-specific checks (planned)
│   └── submodules.py    # 🔄 Git submodule validation (planned)
└── utils/
    ├── __init__.py      # ✅ Utility modules initialization
    ├── installers.py    # ✅ Platform-specific installation guides
    ├── platform.py      # 🔄 Cross-platform detection utilities (planned)
    ├── process.py       # 🔄 Cross-platform subprocess execution (planned)
    └── filesystem.py    # 🔄 Cross-platform file/directory utilities (planned)
```

**Legend**: ✅ Implemented | 🔄 Planned/In Progress

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

**Current Status**: Phase 1 Complete ✅ | Phase 2 In Progress 🔄

### ✅ Implemented Features

- **CLI Framework**: Full Typer + Rich implementation with multiple commands
- **Health Check System**: Async-based modular architecture with dependency management
- **Progress Reporting**: Real-time progress bars, color-coded results, timing information
- **Core Checks**: Platform detection, Python version, Git, CMake, C++20 compiler validation
- **Installation Help**: Platform-specific installation instructions for Windows/macOS/Linux
- **Configuration System**: Flexible config management with environment variable support

### 🔍 Current Capabilities

```bash
# Working commands from Helipad root:
tools/healthy check --verbose              # Run all checks with detailed output (Linux/macOS)
tools\healthy.cmd check --verbose          # Run all checks with detailed output (Windows)
tools/healthy list-checks                  # List available health checks (Linux/macOS)
tools\healthy.cmd list-checks              # List available health checks (Windows)
tools/healthy install-help git             # Get installation help for tools (Linux/macOS)
tools\healthy.cmd install-help git         # Get installation help for tools (Windows)
tools/healthy version                      # Show version and platform info (Linux/macOS)
tools\healthy.cmd version                  # Show version and platform info (Windows)

# Working commands from tools/healthy-cli directory:
uv run healthy check --verbose         # Run all checks with detailed output
uv run healthy list-checks             # List available health checks
uv run healthy install-help git        # Get installation help for tools
uv run healthy version                 # Show version and platform info
```

### 🎯 Next Steps (Phase 2)

1. **Enhanced System Checks**: Improve cross-platform path handling and version parsing
2. **Optional Tool Detection**: Implement Doxygen and ccache checks
3. **Configuration Files**: Support for .healthy.json project configuration
4. **Error Recovery**: Better suggestions and automated fix attempts

### 🚀 Future Considerations

#### Performance Optimizations

- **Parallel Execution**: Run independent checks concurrently to reduce total time
- **Caching**: Cache successful check results to avoid repeated expensive operations
- **Smart Dependencies**: Only run dependent checks when prerequisites change

#### Extensibility Improvements

- **Plugin System**: Allow third-party health checks via plugin mechanism
- **Custom Check Scripts**: Support for user-defined health check scripts
- **Template System**: Configurable check templates for different project types

#### User Experience Enhancements

- **Interactive Mode**: Guided setup wizard for first-time users
- **Progress Persistence**: Resume interrupted health checks
- **Historical Tracking**: Track health check results over time
- **Integration Hooks**: Pre-commit hooks, CI/CD pipeline integration

#### Advanced Features

- **Health Metrics**: Quantitative scoring of environment health
- **Trend Analysis**: Track improvements/degradations over time
- **Team Collaboration**: Shared team health standards and reporting
- **Documentation Generation**: Auto-generate setup documentation from checks

## Contributing

This tool is part of the Helipad game engine project. Follow the project's contribution guidelines and coding standards when implementing features.
