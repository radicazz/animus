# ☂️ Incarnate

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE.txt)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/CMake-3.21%2B-blue.svg)](https://cmake.org/)
[![SDL3](https://img.shields.io/badge/SDL-3.0-red.svg)](https://github.com/libsdl-org/SDL)

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

## 📁 Project Structure

```text
incarnate/
├── assets/                # Game assets (sprites, fonts)
│   ├── fonts/             # TrueType fonts
│   └── sprites/           # Sprite images
├── docs/                  # Documentation generation settings
├── external/              # Git submodules (SDL, GLM, etc.)
├── src/
│   ├── engine/            # Engine core systems
│   │   ├── ecs/           # Entity Component System
│   │   ├── renderer/      # Sprite & Font rendering system
│   │   ├── utils/         # Utility functions
│   │   └── window/        # Window management
│   └── game/              # Development game showcasing features
├── build/                 # Build output (generated)
├── CMakeLists.txt         # Main CMake configuration
└── README.md              # This file
```

## 🎯 Usage Example

```cpp
#include "engine/engine.hxx"

/* Cross-platform SDL entry-point */
#include <SDL3/SDL_main.h>

struct game_state {
    entt::entity player = {entt::null};
};

void game_create(engine::game_engine* engine) {
    auto& state = engine->get_state<game_state>();
    engine::ecs_manager& ecs = engine->get_ecs_manager();
    engine::game_resources& resources = engine->get_resources();

    // Load an image from the assets folder as a sprite.
    auto player_sprite = resources.sprite_create("assets/player.png");

    // Register your player entity with the sprite.
    state.player = ecs.create_sprite_entity({400.f, 300.f}, std::move(player_sprite));

    // Add velocity component for movement.
    ecs.add_component<engine::component_velocity>(state.player) = {
        .linear = {0.0f, 0.0f}, .max_speed = 200.0f, .drag = 0.1f};
}

void game_fixed_update(engine::game_engine* engine, float fixed_delta_time) {
    engine::ecs_manager& ecs = engine->get_ecs_manager();

    // Update physics for all the entities in the engine.
    ecs.update_physics(fixed_delta_time);
}

void game_update(engine::game_engine* engine, float delta_time) {
    auto& state = engine->get_state<game_state>();
    engine::game_input& input = engine->get_input_system();
    engine::ecs_manager& ecs = engine->get_ecs_manager();

    // Make sure the player has not been destroyed.
    if (ecs.is_valid(state.player)) {
        // Access the velocity component.
        auto* velocity = ecs.get_component<engine::component_velocity>(state.player);
        if (velocity) {
            constexpr float acceleration = 500.0f;

            // Use the input system to capture WASD movement.
            glm::vec2 movement(0.0f);

            if (input.is_key_held(engine::input_key::w)) {
                movement.y -= 1.0f;
            }
            if (input.is_key_held(engine::input_key::s)) {
                movement.y += 1.0f;
            }
            if (input.is_key_held(engine::input_key::a)) {
                movement.x -= 1.0f;
            }
            if (input.is_key_held(engine::input_key::d)) {
                movement.x += 1.0f;
            }

            // Apply movement to the player.
            velocity->linear += movement * acceleration * delta_time;
        }
    }
}

void game_render(engine::game_engine* engine, float interpolation_alpha) {
    auto& state = engine->get_state<game_state>();
    engine::game_renderer& renderer = engine->get_renderer();
    engine::ecs_manager& ecs = engine->get_ecs_manager();

    // Render all the sprites in the game.
    ecs.render_sprites(renderer, interpolation_alpha);
}

int main(int argc, char* argv[]) {
    try {
        game_state state;

        // Register your callbacks to hook into the game engine.
        engine::game_info info = {
            .state = &state,
            .on_create = game_create,
            .on_fixed_update = game_fixed_update,
            .on_update = game_update,
            .on_render = game_render
        };

        engine::game_details details = {
            .window_title = "Example",
            .window_size = {1280, 720}
        };

        engine::game_engine game(details, info);

        // Execute the game loop.
        game.run();
    } catch (const std::exception& e) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
```

## 📄 License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE.txt](LICENSE.txt) file for details.

---

Made with lots of ❤️ for fun.
