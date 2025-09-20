# helipad/assets

This directory is used for storing any game/engine assets such as images, sounds, and other resources.

## Usage

Assets placed in this folder will be easily locatable by the game engine at runtime through relative paths. For example, if you add an image called `sprite.png` to this folder like so:

```bash
helipad/
└── assets/
    └── sprite.png      # Example image file.
```

In your game code, you can load this asset using the path `assets/sprite.png` just like this:

```cpp
// Get a reference to the resource manager.
auto& resources = engine->get_resources();

// Create a sprite called "example" from our assets folder.
resources.sprite_get_or_create("example", "assets/sprite.png");
```

## About

During the build process, the entire folder gets copied to the executable's output directory, placing it next to the final binary like this:

```bash
# Example Release build.
Release/
├── helipad.exe     # Game executable.
└── assets/         # Copied assets folder.
```

This setup ensures that all your assets are predictably located relative to your executable, making it easy to manage and access them during development and after deployment.

## Structure

You can organize the `assets` folder in any way that suits your project. A common approach is to create subdirectories for different types of assets for example:

```bash
assets/
├── fonts/
│   └── etc...
├── icons/                     
│   └── etc...
├── sounds/                     
│   └── etc...
└── etc...
```

---

> [!NOTE]
> This folder and its API are planned to be expanded in the future to include more features such as asset packing, hot-reloading, and more.
