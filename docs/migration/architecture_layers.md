# Helipad Architecture Layers

> Notes for a modular C++20 SDL3 2D engine aimed at top-down games, future editor integration, and optional scripting.

## Vision & Guiding Principles

- **Portable**: SDL3 + standard library only at the lowest level; everything else clean C++20.
- **Composable**: systems talk through narrow interfaces (handles, contexts, callbacks) instead of raw pointers.
- **Observable**: each layer exposes lifecycle hooks suitable for editor tools and automated tests.
- **Extensible**: adding a new render pass, scripting VM, or network layer does not rewrite core services.

## High-Level Stack

```
+------------------------------------------------------+
|                    helipad::editor                   |
|  (optional tools, scripting console, asset pipeline) |
+-----------------------------+------------------------+
|     helipad::runtime        |      helipad::services |
|  - frame loop orchestrator  |  - window_manager      |
|  - scene manager            |  - render_manager      |
|  - game/application glue    |  - input_manager       |
+-----------------------------+------------------------+
|            helipad::engine (domain layer)            |
|   scenes | ecs | resource_cache | physics | audio    |
+-----------------------------+------------------------+
|           helipad::render / ::io / ::assets          |
|    (backend facades; renderer device, text, audio)   |
+------------------------------------------------------+
|                 helipad::platform (SDL3)             |
| application | window | renderer_device | input_pump  |
+------------------------------------------------------+
```

Each upper layer depends only on the layer(s) below it. Nothing else ever includes SDL headers.

## Platform Layer (`helipad::platform`)

Responsible for talking to SDL directly. Everything here is RAII, noexcept moveable, and free of engine semantics.

```cpp
// platform/window.hxx
namespace helipad::platform {
    class window {
    public:
        struct create_info {
            std::string title;
            glm::ivec2 size {1280, 720};
            bool high_dpi {true};
        };

        explicit window(const create_info& info);
        ~window();

        window(window&&) noexcept;
        window& operator=(window&&) noexcept;

        SDL_Window* native() const noexcept { return m_window; }
        glm::ivec2 size_pixels() const;
        float dpi_scale() const;

        void set_title(std::string_view title);
        void toggle_fullscreen(bool enabled);

    private:
        SDL_Window* m_window {nullptr};
    };
}
```

SDL init/quit lives inside `platform::application`:

```cpp
class application {
public:
    application() {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
        TTF_Init();
    }
    ~application() {
        TTF_Quit();
        SDL_Quit();
    }
};
```

## Device Facade Layer (`helipad::render`, `helipad::io`)

Wraps platform primitives and provides engine-level handles.

```cpp
// render/texture_registry.hxx
template <typename Tag> class handle { /* 32-bit strong ID */ };
using texture_handle = handle<struct texture_tag>;

class texture_registry {
public:
    explicit texture_registry(platform::renderer_device& device);

    texture_handle load_png(std::string_view path);
    void unload(texture_handle id);

    SDL_Texture* native(texture_handle id) const;

private:
    platform::renderer_device* m_device;
    std::vector<SDL_Texture*> m_textures;
};
```

Renderer device interface consumed by higher layers:

```cpp
class device {
public:
    struct draw_sprite {
        texture_handle texture;
        glm::vec2 position;
        glm::vec2 size;
        glm::vec2 origin {0.5f};
        float rotation {0.f};
        glm::vec4 tint {1.f};
    };

    virtual void begin_pass(glm::vec4 clear_color) = 0;
    virtual void submit(const draw_sprite&) = 0;
    virtual void end_pass() = 0;
};
```

SDL-backed implementation lives in one `.cxx` file. Future Vulkan/Metal devices implement the same interface.

## Runtime Services (`helipad::services`)

These are singleton-like managers owned by the runtime. They wire scenes to devices.

```cpp
// services/window_manager.hxx
class window_manager {
public:
    explicit window_manager(platform::application& app,
                            platform::window::create_info info);

    platform::window& main_window() noexcept;
    std::span<const platform::window*> windows() const;

    connection on_resize(std::function<void(glm::ivec2)> cb);

private:
    platform::application* m_app;
    std::vector<std::unique_ptr<platform::window>> m_windows;
    signal<glm::ivec2> m_resize_signal;
};
```

`render_manager` consumes the queue and device:

```cpp
class render_manager {
public:
    explicit render_manager(render::device& device);

    struct render_item {
        render::texture_handle texture;
        glm::mat4 transform;
        glm::vec4 tint;
        uint8_t layer;
    };

    void submit(render_item item);

    void execute_passes(const std::vector<render_view>& views) {
        for (auto& pass : views) {
            m_device.begin_pass(pass.clear_color);
            for (const auto& item : m_queue[pass.id]) {
                render::device::draw_sprite sprite {
                    .texture = item.texture,
                    .position = glm::vec2(item.transform[3]),
                    .size = glm::vec2(item.transform[0][0], item.transform[1][1]),
                    .rotation = extract_rotation(item.transform),
                    .tint = item.tint
                };
                m_device.submit(sprite);
            }
            m_device.end_pass();
        }
        clear_queue();
    }

private:
    render::device& m_device;
    std::array<std::vector<render_item>, max_passes> m_queue;
};
```

## Engine Domain (`helipad::engine`)

Scenes, ECS, resources, scripting live here. They know nothing about SDL. They receive services through contexts.

```cpp
struct scene_context {
    ecs::world& world;
    resource_cache& resources;
    render_manager& renderer;
    input_manager& input;
    float delta_s;
};

using scene_tick_cb = std::function<void(scene_context&)>;

class scene {
public:
    scene(std::string name, scene_callbacks callbacks);

    void on_tick(scene_context&);
    void on_frame(scene_context&);
    void on_input(scene_context&, const input_snapshot&);

private:
    std::string m_name;
    scene_callbacks m_callbacks;
    ecs::world m_world;
};
```

`engine::runtime` drives the loop:

```cpp
class runtime {
public:
    runtime(services::window_manager&, services::render_manager&, services::input_manager&);

    void run(scene& start_scene) {
        scene* active = &start_scene;
        while (!m_should_quit) {
            auto dt = m_clock.frame_delta();
            m_input.update();

            scene_context ctx {
                .world = active->world(),
                .resources = m_resources,
                .renderer = m_renderer,
                .input = m_input,
                .delta_s = dt.seconds
            };

            active->on_input(ctx, m_input.snapshot());
            while (m_fixed_accumulator.consume(dt)) {
                active->on_tick(ctx);
            }
            active->on_frame(ctx);

            m_renderer.execute_passes(m_view_registry.active_views());
        }
    }
};
```

## State Flow (One Frame)

1. **Platform** polls SDL events -> `io::input_system` stores state.
2. **Runtime** builds `scene_context` (world + services + delta) and dispatches callbacks.
3. Scene/ECS run systems (physics, scripting) mutating component data.
4. Rendering systems push `render_manager::render_item` entries (resource handles + transforms).
5. `render_manager` sorts items per pass/layer and tells `render::device` to draw.
6. Device issues SDL draw calls; platform swaps the window.

```
SDL Events -> platform::input_pump -> input_manager
           -> window_manager resize signal -> view_registry
ECS Systems -> render_manager.submit -> render_queue
render_manager.execute -> render::device -> SDL
```

## Why This Split Scales

- **Maintainability**: each layer has a single reason to change (SRP). Updating SDL code never touches gameplay logic.
- **Testability**: mock `render::device` to unit test render systems; mock `input_manager` to simulate input in scene tests.
- **Cross-platform**: only `platform` depends on SDL/OS APIs. Porting to a new backend or adding Metal/Vulkan means writing a new device module, not rewriting the engine.
- **Editor-ready**: editor can embed its own passes (gizmos) by pushing items into the render queue or registering additional views without altering gameplay code.
- **Scripting-friendly**: runtime can expose contexts to Lua/Rust/AngelScript without leaking SDL pointers.

## Future Extensions

- **Multiple Windows**: window_manager already manages a vector -> open an editor viewport alongside the game.
- **Render Targets**: extend `render::device` with `create_render_target`, enabling post-processing or minimaps.
- **Hot Reloading**: resource_cache watches files, reuploads textures while scenes continue to use handles.
- **Networking**: introduce `helipad::net` alongside `services` without touching platform layer.

## Summary

Layering the engine into platform -> device -> services -> runtime -> editor keeps SDL isolated, stabilises the API surface, and leaves headroom for tooling, scripting, and alternative backends. The C++20 features (structured bindings, `std::span`, strong typedef handles) make the interfaces expressive without virtual inheritance in gameplay code. This blueprint should be the reference when implementing or reviewing Helipad modules.
