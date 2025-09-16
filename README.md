# helipad

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![SDL3](https://img.shields.io/badge/SDL-3.0-green.svg)](https://github.com/libsdl-org/SDL)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-orange.svg)](LICENSE.txt)

A light-weight, modular and easy-to-use top-down 2D game engine built from a different perspective.

## Features

> This project is still in early development, features are constantly being added and improved.

- Cross-platform support for Windows, Linux, and macOS.
- Modular architecture with a focus on extensibility and maintainability.
- Resource management system for efficient handling of assets like textures, sprites and fonts.
- Entity-Component-System (ECS) integration for flexible game object management.
- Sprite & text rendering with support for rotations, scaling and more.

## Getting Started

Third-party libraries are stored in the [`external/`](external/) directory.

### Requirements

Before you can build and run the examples, make sure you have the following tools installed:

- **Compiler** - A C++20 compatible compiler.
  - [GCC](https://gcc.gnu.org/) (Linux, MinGW on Windows)
  - [Clang](https://clang.llvm.org/) (Linux, macOS)
  - [MSVC](https://visualstudio.microsoft.com/) (Windows)
- **[CMake](https://cmake.org/)** - Cross-platform build system.
- **[Git](https://git-scm.com/downloads)** - Version control system for managing submodules.

And optionally:

- **[Ccache](https://ccache.dev/)** - A compiler cache to speed up recompilation.
- **[Doxygen](https://www.doxygen.nl/index.html)** - Engine API documentation generator.

### Building

Get started by cloning this repository with the `--recursive` flag to include submodules:

```bash
git clone --recursive https://github.com/radicazz/helipad.git
cd helipad
```

> If you already cloned the repository without `--recursive`, you can still initialize and update the submodules manually with `git submodule update --init --recursive`.

Once you have the project and its dependencies, its time to configure and build for your platform:

```bash
# Configure the project in Debug mode.
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug # or Release.
cmake --build build
```

If everything went well, you will find the final executable, shared libraries and assets in the `/build/bin/` directory.

> If you run into errors during the configuration or build process, the most most likely cause is missing dependencies. Keep in mind that submodules can have submodules of their own which have their own dependencies. Be sure to check CMake's output for clues on what might be missing.

## Documentation

If you have [Doxygen](https://www.doxygen.nl/index.html) installed, you can generate the API documentation by either of the following methods:

- Automatically during the build process:
  - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENGINE_ENABLE_DOCS=ON`
- Or manually from the root directory:
  - `doxygen Doxyfile`

Either way, the generated documentation will be available in the `build/docs/html` directory.

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE.txt](LICENSE.txt) file for details.
