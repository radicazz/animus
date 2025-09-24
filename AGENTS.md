# Helipad Agent Atlas

> Work-in-progress C++20 SDL3 engine. Use this as a navigation crib; confirm details in source before automation.

## Project Map

```
helipad/
|-- src/
|   |-- engine/
|   |   |-- renderer/        (drawing, cameras, viewports)
|   |   |-- utils/           (input, resources, timing, scenes)
|   |   |-- ecs/             (entt wrappers and systems)
|   |   |-- logger.hxx       (logging helpers)
|   |   `-- config.hxx.in    (CMake-configured constants)
|   `-- game.cxx             (demo wiring for callbacks + ECS)
|-- assets/                  (fonts, sprites, icons; copied post-build)
|-- docs/                    (developer docs, quick start)
|-- external/                (SDL3, SDL_image, SDL_ttf, glm, entt)
`-- tools/                   (Python utilities in progress)
```

## Runtime Core Snapshot

```
game_entry_point()
  -> constructs engine::game_engine
       |-- game_window        (SDL window lifecycle)
       |-- game_renderer      (SDL renderer + text engine)
       |-- game_input         (event processing, key state sets)
       |-- game_scenes        (scene registry + dispatch)
       `-- utils::timing      (high-resolution counters)
```

- Lifecycle hooks live in `game_engine_callbacks` (`src/engine/engine.hxx`).
- `engine::safe_invoke` and `engine::ensure` (`src/engine/safety.hxx`) wrap optional callbacks and paranoid assertions.
- Main loop in `src/engine/engine.cxx::game_engine::start_running` drives fixed tick plus variable frame stages.

## Scene & ECS Flow

```
engine loop stages -> game_scenes -> active game_scene
    game_scene state (user owned)
    |-- game_entities (entt registry + helpers)
    |   |-- system_physics_update   (linear/angular velocity)
    |   |-- system_lifetime_update  (expiry)
    |   `-- system_renderer_update  (submit sprites/text)
    |-- game_resources              (sprites, fonts, text caches)
    `-- camera/viewport registries  (currently unordered_map, TODO: vector)
```

- Load scenes with `game_scenes::load_scene` then `activate_scene` (`src/engine/utils/scenes.hxx/.cxx`).
- ECS helpers (`src/engine/ecs/entities.hxx/.cxx`) provide creation, component access, interpolation, and impulse utilities.

## Rendering Stack

```
assets/ -> game_resources -> {game_sprite | game_text_*}
                                |
                                v
                          game_renderer
                                |
                                v applies camera + viewport transforms
                          SDL_Renderer (clears, draws, presents)
```

- Renderer implementation: `src/engine/renderer/renderer.hxx/.cxx`.
- Cameras: `src/engine/renderer/camera.hxx/.cxx` (zoom, bounds, follow helpers).
- Viewports: `src/engine/renderer/viewport.hxx/.cxx` (world to screen transforms, clamping).

## Input & Interaction

```
SDL events -> game_input::process_sdl_event
    |-- pressed/held/released sets (per frame)
    |-- movement helpers (WASD, arrow keys)
    `-- mouse utilities (screen position, delta)
```

- `game_input_key` enum and mappings live in `src/engine/utils/input.hxx`.
- Demo (`src/game.cxx`) shows toggling camera modes, zoom keys, and viewport-based mouse picking.
- Input mapping overhaul is noted as a TODO; avoid assuming the enum is final.

## Build & Configuration

```
CMakeLists.txt
    |-- finds/links SDL3, SDL_image, SDL_ttf submodules
    |-- configures generated/config.hxx from src/engine/config.hxx.in
    |-- options: ENGINE_LOG_INFO/WARNING/ERROR, ENGINE_PARANOID
    |-- enforces C++20, high warning levels, optional ccache
    `-- copies assets/ -> build/bin/assets (post-build)
```

- Configure docs with `-DENGINE_ENABLE_DOCS=ON`; run `doxygen Doxyfile` for HTML under `build/docs/html`.
- Windows builds copy SDL DLLs; Unix-like targets set `$ORIGIN` RPATH.

## Style & Documentation Signals

```
STYLEGUIDE.md
    - snake_case for everything (types included)
    - m_ prefix for class data members
    - enum class only, no raw enums
    - headers use #pragma once and ordered include blocks
    - prefer RAII, concepts, std::format, std::optional
```

- Public-facing headers (e.g., `src/engine/engine.hxx`, renderer headers) already follow Doxygen comment patterns; mirror them for new APIs.

## Quick Targets for Agents

- Orientation: `README.md`, `docs/README.md`, `STYLEGUIDE.md`.
- Core runtime: `src/engine/engine.hxx`, `src/engine/engine.cxx`, `src/engine/safety.hxx`.
- Scene/ECS: `src/engine/utils/scenes.hxx/.cxx`, `src/engine/ecs/entities.hxx/.cxx`.
- Rendering: `src/engine/renderer/renderer.cxx`, `viewport.cxx`, `camera.cxx`.
- Gameplay example: `src/game.cxx` (callbacks, ECS usage, input wiring).
- Asset contract: `assets/README.md` defines runtime expectations.

## Automation Cautions

- Project is fast-moving; verify signatures before code generation.
- Entt systems expect required components; use `has` / `try_get` before accessing.
- Dynamic text updates trigger texture regeneration; batch changes to avoid stalls.
- Cameras/viewports and input mapping have explicit TODO refactors; keep integrations adaptable.
