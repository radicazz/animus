# Renderer Modularization & Extensibility Plan

## Purpose

Helipad's current renderer works for the demo, but it tightly couples resource data, render state, and SDL calls. This plan defines the incremental work needed to make the renderer modular, testable, and ready for features like multi-view rendering, batching, and editor tooling.

## Current Pain Points

- **Shared mutable resources**: sprites/text objects are reused per entity, so drawing changes their rotation/scale globally.
- **Global state**: `game_renderer` stores camera/viewport pointers and its own viewport map while scenes keep another, creating ownership ambiguity.
- **No submission stage**: systems call SDL immediately, preventing ordering, batching, and multi-pass effects.
- **Tight coupling to SDL**: the renderer mixes engine policy with backend details, making alternate backends or off-screen passes difficult.
- **Weak layering support**: render order relies on registry iteration; there's no stable layer or z-order guarantee.

## Target Architecture Overview

```
+---------------------------+          +-------------------------------+
|        ecs::systems       |          |       resource_cache           |
| (collect renderables)     |          |  (immutable textures/fonts)    |
+-------------+-------------+          +-------------------------------+
              |                                        ^
              v                                        |
+---------------------------+          +-------------------------------+
|       render_queue        | -------> |    render_device (SDL/HW)      |
| (sorted render_items)     |          |  - submit sprites/text         |
+-------------+-------------+          |  - manages passes/targets      |
              |                          +---------------+---------------+
              v                                          ^
+---------------------------+          +-------------------------------+
|   render_facade (game_)   | <------- |   view_registry (camera+vp)    |
|  - accepts frame context  |          +-------------------------------+
|  - drives passes begin/end|
+---------------------------+
```

### Key Concepts

- **Immutable resources**: textures/fonts live in a cache; instances carry transforms/tints only.
- **Render queue**: ECS systems push `render_item` structs (resource handle, transform, layer) into a per-frame queue, then sort by layer/material.
- **Render facade**: `game_renderer` becomes a stateless entry point that consumes a queue and view definitions, leaving device specifics to a lower layer.
- **View registry**: a single source of truth for cameras/viewports that scenes/editors update via handles.
- **Render device**: thin wrapper around SDL (later Vulkan/Metal) that exposes minimal operations (create target, submit quad, blit text).

## Phase Plan

### Phase 0 � Prep & Baseline

- Document current renderer, resource manager, and ECS render system behaviour.
- Add safety tests/smoke tests that validate today's behaviour (single sprite draw, text draw, main viewport clears).
- **Outcome**: we can detect regressions as we refactor.

### Phase 1 � Resource Hygiene

- Make `game_sprite`, `game_text_*` immutable (remove per-frame `set_rotation`/`set_scale` usage).
- Introduce lightweight `sprite_instance` / `text_instance` structs inside ECS system (holds transform, scale, tint).
- Update `game_resources` to return handles (e.g., `sprite_id`) instead of raw pointers; keep pointer access for transition via adapters.
- **Why**: stop data races on shared resources and prepare for instancing.
- **Acceptance**: system renderer no longer writes to shared resource objects; rendering still works for the demo.

### Phase 2 � Render Queue

- Introduce `render_item` struct: `{ pass_id, layer, resource_handle, transform, color, flags }`.
- Add `render_queue` container with `submit(item)` and `flush()` (sort by `pass_id`, `layer`).
- Update `system_renderer::update` to push items instead of calling `sprite_draw_*` directly.
- Modify `game_renderer::draw_begin/draw_end` to consume items from the queue.
- **Why**: centralises ordering & enables batching. Sets foundation for multi-view, materials, and UI overlays.
- **Acceptance**: existing example renders using queue; sort order respects `component_renderable::layer`.

### Phase 3 � View Ownership

- Create `render_view` struct: `{ camera_handle, viewport_handle, pass_id, clear_color }`.
- Move viewport registry out of `game_renderer` into a dedicated `view_registry` owned by scenes (or engine) with stable handles.
- Update scenes to register per-view each frame (e.g., main view, UI view); renderer consumes handles from queue items.
- Remove legacy single camera/viewport pointers from `game_renderer`.
- **Why**: Single source of truth avoids dangling pointers and supports multiple views per frame.
- **Acceptance**: scene activation sets active view by handle; multiple viewports render sequentially using queue filters.

### Phase 4 � Render Device Abstraction

- Introduce `render_device` interface with SDL-backed implementation. Responsibilities:
  - Manage SDL renderer and text engine lifetime
  - Provide `begin_pass`, `submit_quad`, `submit_text`, `end_pass`
  - Query output size
- `game_renderer` becomes a thin coordinator: iterates render queue, calls device operations, handles pass boundaries.
- Update `game_resources` to request textures/fonts from `render_device` (encapsulate SDL pointers in RAII wrappers).
- **Why**: isolates backend and simplifies testing; future-proofs for off-screen passes or GPU transitions.
- **Acceptance**: renderer files no longer call SDL directly except inside device implementation; tests can mock the device.

### Phase 5 � Extensions & Hardening

- Add optional `render_pass` definitions (world, UI) with configurable clear state and target.
- Support `render_item` flags for screen-space vs world-space, tinted sprites, flip modes.
- Add visibility culling hook (e.g., allow systems to skip queueing if outside frustum; later integrate broadphase).
- Integrate basic metrics/logging (queue size, draw call count) for debugging.
- Write design-driven tests/integration checks (render queue sorts correctly, removing view handles detaches from renderer).

## Migration & Coordination

- **Incremental**: Each phase keeps examples runnable; resource API adapters bridge old/new handles during transition.
- **Parallel work**: While Phase 2 is in flight, resource team can begin handle wrappers; once queue exists, ECS and renderer can evolve separately.
- **Deprecation plan**: Mark current `set_camera/set_viewport` as deprecated after Phase 3; remove once view handles adopted.

## Risks & Mitigations

- **API churn for `game_resources`**: Provide compatibility path with adapter methods until callers migrate.
- **Performance regressions**: Benchmark before/after queue introduction; ensure queue allocations are pooled.
- **Complexity creep**: Keep render queue minimal (POD items, simple sort) before adding fancy features like materials or instancing.

## Success Criteria

- Renderer exposes clear boundaries: resource cache, queue, device.
- Multiple viewports/cameras per frame without reconfiguring global pointers.
- ECS systems submit render data without mutating shared assets.
- SDL-specific code isolated to one translation unit (`render_device_sdl.cxx`).
- Future additions (editor overlays, minimap, post-processing) can be implemented by adding passes rather than rewriting core components.

## Future Opportunities

- **Batching/instancing**: With immutable resources and queue, grouping by texture is straightforward.
- **Off-screen targets**: Extend render device to create textures as render targets (needed for UI blur or minimaps).
- **Threaded culling**: Queue submissions can be built in parallel once data is detached from SDL.
- **Editor integration**: Editor can register its own passes without interfering with gameplay rendering.

---
*Owner*: renderer team  �  *Last updated*: {{DATE:YYYY-MM-DD}}
