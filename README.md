# ☂️ Incarnate

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/CMake-3.21%2B-blue.svg)](https://cmake.org/)
[![SDL3](https://img.shields.io/badge/SDL-3.0-red.svg)](https://github.com/libsdl-org/SDL)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE.txt)

A cross-platform 2D game/engine built with modern C++ & SDL.

## ✨ Features

- **Input System**: Keyboard and mouse input handling with customizable key mappings.
- **Sprite Rendering**: Hardware-accelerated sprite rendering with rotation and scaling.
- **Text Rendering**: TTF font rendering with support for rotation and scaling.
- **Resource Management**: Automatic texture loading and memory management.
- **Mathematics**: GLM integration for 2D transformations and calculations.
- **Modular Architecture**: Clean separation between game logic and engine systems.

## 📋 Requirements

- **Compiler**: C++20 compatible compiler (MSVC 2019+, GCC 10+, Clang 10+)
- **[Git](https://git-scm.com/downloads)** - Version control for cloning submodules
- **[CMake 3.21+](https://cmake.org/)** - Build system
- **Platform-specific build tools**:
  - **Windows**: Visual Studio 2019+ or Visual Studio Build Tools
  - **macOS**: Xcode Command Line Tools
  - **Linux**: GCC/G++ and development libraries

## 🚀 Quick Start

### Clone

```bash
# Clone with all submodules.
git clone --recursive https://github.com/radicazz/incarnate.git
cd incarnate
```

In case you forgot to clone with submodules, you can initialize them with:

```bash
git submodule update --init --recursive
```

### Build

```bash
# Make a build directory.
mkdir build && cd build

# Configure the project.
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build the project.
cmake --build .
```

### Run

You should find your compiled libraries and executable in the `build/` directory.

## 📄 License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE.txt](LICENSE.txt) file for details.

---

Made with lots of ❤️ for fun.
