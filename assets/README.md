# helipad/assets

Use this directory to store, organize and/or manage any assets required by your project.

During the build process, the entire folder gets copied to the executable's output directory, placing it next to the final binary like this:

```bash
# Example Release build.
Release/
├── helipad.exe     # Game executable.
├── assets/         # Copied assets folder.
│   └── sprite.png
└── etc...
```

Once the assets are located in the output folder, its easy to load them using relative paths. For example, to create a sprite from the `sprite.png` file above, you would do something like this:

```cpp
void on_create(engine::game_engine* engine) {
    // Get a reference to the resource manager.
    auto& resources = engine->get_resources();

    // Create a sprite called "player" from our assets folder.
    resources.sprite_get_or_create("player", "assets/player.png");
}
