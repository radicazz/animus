# Incarnate

A cross-platform game that I'm not sure about yet, made with <3

## Todo

- Create a `Sprite` class to represent sprites in the game.
- Add a `BaseEntity` class to represent world-space entities. (Maybe ECS?)
- Complete and implement the `Camera` into the engine.

## Requirements

- C++20 compatable compiler.
- [Git](https://git-scm.com/downloads) - Version control.
- [CMake 3.21+](https://cmake.org/) for compiling.

## Dependencies

> The following dependencies are added as submodules to the project. They may have their own dependencies that you need to consider.

- [SDL3](https://github.com/libsdl-org/SDL) - Window creation, primitive rendering & system input.
- [SDL3_image](https://github.com/libsdl-org/SDL_image) - Image texture loading & creation.
- [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf) - TTF font loading & rendering.
- [glm](https://github.com/g-truc/glm) - Mathematics library to simplify my life.
