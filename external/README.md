# animus/external

This directory contains all the third-party dependencies used by the project.

## About

All of the third-party dependencies are included as [Git](https://git-scm.com/) submodules within this directory. Managing these dependencies as submodules means that they will automatically be compiled and linked for the platform they are being built on, ensuring cross-platform compatibility.

## Dependencies

### Header Only

- **[EnTT](https://github.com/skypjack/entt)** - Fast Entity-Component System. (ECS)
- **[GLM](https://github.com/g-truc/glm)** - OpenGL Mathematics library for 2D/3D math.

### Compiled

- **[SDL3](https://github.com/libsdl-org/SDL)** - Window management, rendering, and input.
- **[SDL3_image](https://github.com/libsdl-org/SDL_image)** - Image loading. (PNG, JPG, etc...)
- **[SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)** - TrueType font rendering.
