# helipad/docs

Documentation for the Helipad 2D game engine, including a quick start guide and API reference.

---

## Requirements

Before you can build and run the engine and its examples, you need to have the following tools installed on your system:

- **Compiler** - A C++20 compatible compiler.
  - [GCC](https://gcc.gnu.org/) (Linux, MinGW on Windows)
  - [Clang](https://clang.llvm.org/) (Linux, macOS)
  - [MSVC](https://visualstudio.microsoft.com/) (Windows)
- **[CMake](https://cmake.org/)** - Cross-platform build system.
- **[Git](https://git-scm.com/downloads)** - Version control system for managing submodules.

Optionally, you can install the following tools to improve your development experience:

- **[Ccache](https://ccache.dev/)** - A compiler cache to speed up recompilation.
- **[Doxygen](https://www.doxygen.nl/index.html)** - Engine API documentation generator.
- **[Python](https://www.python.org/)** - Required for some project utilities.

---

## Building

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

---

## Documentation

If you have [Doxygen](https://www.doxygen.nl/index.html) installed, you can generate the API documentation by either of the following methods:

- Automatically during the build process:
  - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENGINE_ENABLE_DOCS=ON`
- Or manually from the root directory:
  - `doxygen Doxyfile`

Either way, the generated documentation will be available in the `build/docs/html` directory.

---

Made with lots of ❤️
