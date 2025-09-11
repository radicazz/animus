# Incarnate

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![SDL3](https://img.shields.io/badge/SDL-3.0-green.svg)](https://github.com/libsdl-org/SDL)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-orange.svg)](LICENSE.txt)

A cross-platform top-down 2D game/engine built with modern C++ & SDL.

## Requirements

> Dependencies are located in [`/external/`](external/) and managed as git submodules.

- **Compiler** - C++20 compatible compiler.
- **[Git](https://git-scm.com/downloads)** - Version control system for managing submodules.
- **[CMake](https://cmake.org/)** - Cross-platform build system.

## Quick Start

Clone this repository with the `--recursive` flag to automatically include submodules:

```bash
git clone --recursive https://github.com/radicazz/incarnate.git
cd incarnate
```

> Use `git submodule update --init --recursive` to fetch the submodules manually.

Configure and build the project (this example uses a debug build):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build
```

The binaries will be located in the `/build/` directory.

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE.txt](LICENSE.txt) file for details.
