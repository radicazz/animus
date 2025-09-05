# 🎮 Incarnate

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE.txt)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/CMake-3.21%2B-blue.svg)](https://cmake.org/)
[![SDL3](https://img.shields.io/badge/SDL-3.0-red.svg)](https://github.com/libsdl-org/SDL)

A cross-platform 2D game engine built with modern C++, SDL, and love ❤️

## ✨ Features

- **🎯 2D Game Engine**: Complete camera system with following, bounds, and zoom
- **🎮 Input System**: Keyboard and mouse input handling with customizable key mappings
- **🖼️ Sprite Rendering**: Hardware-accelerated sprite rendering with rotation and scaling
- **🎨 Resource Management**: Automatic texture loading and memory management
- **📐 Mathematics**: GLM integration for 2D transformations and calculations
- **🏗️ Modular Architecture**: Clean separation between game logic and engine systems
- **⚡ Cross-Platform**: Windows, macOS, and Linux support

## 🎮 Demo Controls

The included demo showcases the engine's capabilities:

- **WASD** - Move player
- **C** - Toggle between camera follow mode and free camera mode
- **Arrow Keys** - Move camera (free camera mode only)
- **O** - Zoom out
- **P** - Zoom in
- **ESC** - Exit

## 📋 Requirements

- **Compiler**: C++20 compatible compiler (MSVC 2019+, GCC 10+, Clang 10+)
- **[Git](https://git-scm.com/downloads)** - Version control for cloning submodules
- **[CMake 3.21+](https://cmake.org/)** - Build system
- **Platform-specific build tools**:
  - **Windows**: Visual Studio 2019+ or Visual Studio Build Tools
  - **macOS**: Xcode Command Line Tools
  - **Linux**: GCC/G++ and development libraries

## 🔧 Dependencies

All dependencies are included as Git submodules and built automatically:

- **[SDL3](https://github.com/libsdl-org/SDL)** - Window management, rendering, and input
- **[SDL3_image](https://github.com/libsdl-org/SDL_image)** - Image loading (PNG, JPG, etc.)
- **[SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)** - TrueType font rendering
- **[GLM](https://github.com/g-truc/glm)** - OpenGL Mathematics library for 2D/3D math

## 🚀 Quick Start

### Clone the Repository

```bash
# Clone with all submodules
git clone --recursive https://github.com/radicazz/incarnate.git
cd incarnate

# If you forgot --recursive, initialize submodules:
git submodule update --init --recursive
```

### 🖥️ Build

```batch
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
.\Debug\incarnate.exe
```

## 📁 Project Structure

```text
incarnate/
├── assets/                # Game assets (sprites, fonts)
│   ├── fonts/             # TrueType fonts
│   └── sprites/           # Sprite images
├── external/              # Git submodules (SDL, GLM, etc.)
├── src/
│   ├── engine/            # Engine core systems
│   │   ├── camera/        # 2D camera with following and bounds
│   │   ├── input/         # Input handling system
│   │   ├── renderer/      # Sprite rendering system
│   │   └── resource/      # Asset loading and management
│   └── game/              # Demo game implementation
├── build/                 # Build output (generated)
├── CMakeLists.txt         # Main CMake configuration
└── README.md              # This file
```

## 🎯 Usage Example

```cpp
#include "engine/engine.hxx"

struct my_game_state {
    glm::vec2 player_position;
    engine::game_sprite* player_sprite;
};

void game_create(engine::game_engine* engine) {
    auto& state = engine->get_state<my_game_state>();
    state.player_position = {400, 300};
    
    auto& resource_manager = engine->get_resource_manager();
    state.player_sprite = resource_manager.sprite_load("player.png");
}

void game_update(engine::game_engine* engine, float delta_time) {
    auto& state = engine->get_state<my_game_state>();
    auto& input = engine->get_input_system();
    auto& camera = engine->get_camera();
    
    // Handle input and update game state
    // Camera automatically handles bounds and transformations
    camera.follow_target(state.player_position);
}

void game_render(engine::game_engine* engine) {
    auto& state = engine->get_state<my_game_state>();
    auto& renderer = engine->get_renderer();
    
    renderer.sprite_draw_world(state.player_sprite, state.player_position);
}
```

## 📄 License

This project is licensed under the MIT License - see the [LICENSE.txt](LICENSE.txt) file for details.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit issues, feature requests, or pull requests.

---

Made with ❤️ using modern C++ and SDL.
