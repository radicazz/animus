# Incarnate

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![SDL3](https://img.shields.io/badge/SDL-3.0-green.svg)](https://github.com/libsdl-org/SDL)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-orange.svg)](LICENSE.txt)

A cross-platform top-down 2D game/engine built to explore the complexities of game development from a hands-on perspective.

## Getting Started

Much of this project relies on third-party libraries for core functionality such as window management, rendering, mathematics operations, and more. These dependencies are included as Git submodules within the [`external`](external/) directory which you can take a look at. Managing these dependencies as submodules means that they will automatically be compiled and linked for the platform they are being built on.

### Requirements

To build this game and its submodules, you will need:

- **Compiler** - A C++20 compatible compiler.
  - [GCC](https://gcc.gnu.org/) (Linux, MinGW on Windows)
  - [Clang](https://clang.llvm.org/) (Linux, macOS)
  - [MSVC](https://visualstudio.microsoft.com/) (Windows)
- **[CMake](https://cmake.org/)** - Cross-platform build system.
- **[Git](https://git-scm.com/downloads)** - Version control system for managing submodules.

### Quick Start

Get started by cloning this repository with the `--recursive` flag to include submodules:

```bash
git clone --recursive https://github.com/radicazz/incarnate.git
cd incarnate
```

> If you already cloned the repository without `--recursive`, you can still initialize and update the submodules manually with `git submodule update --init --recursive`.

Once you have the project and its dependencies, its time to configure the project and build for your platform:

```bash
# Configure the project in Debug mode.
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug # or Release.
cmake --build build
```

If everything went well, you will find the final executable, shared libraries and assets in the `/build/bin/` directory.

> If you run into errors during the configuration or build process, the most most likely cause is missing dependencies. Keep in mind that submodules can have submodules of their own which have their own dependencies. Be sure to check CMake's output for clues on what might be missing.

### Documentation

This project is configured to generate documentation from the source code using [Doxygen](https://www.doxygen.nl/index.html).

Once you have it installed and available on your path, there are two ways to generate the project's documentation:

1. Enable the `INCARNATE_ENABLE_DOCS` option when configuring the project with CMake:
    - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DINCARNATE_ENABLE_DOCS=ON`
    - `cmake --build build`
2. or run Doxygen directly from the root directory:
    - `doxygen Doxyfile`

Either way, the generated documentation will be available in the `build/docs/html` directory.

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE.txt](LICENSE.txt) file for details.
