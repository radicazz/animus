# C++20 Patterns for Helipad

> Pragmatic notes on leveraging C++20 in an SDL3-based 2D engine while following Helipad's style (snake_case, `m_` members), keeping the runtime lean, and avoiding exception-heavy or `std::function`-heavy designs.

## Goals & Constraints

- **Performance aware**: avoid hidden heap allocations (`std::function`) and prefer flat data-oriented structures.
- **Exception-light**: hot paths should return explicit status codes/objects; reserve exceptions for configuration-time failures.
- **Styleguide alignment**: snake_case names, `enum class`, RAII wrappers, zero-cost abstractions.
- **SDL-friendly**: wrap SDL types in C++ RAII while keeping APIs close to existing SDL semantics.

## 1. Designated Initializers for SDL Interop

C++20 designated initializers keep SDL structs readable without helper builders.

```cpp
SDL_Window* create_window(const glm::ivec2 size) {
    const SDL_WindowFlags flags = static_cast<SDL_WindowFlags>(
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    return SDL_CreateWindow("Helipad",
                            size.x,
                            size.y,
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            flags);
}

SDL_RendererInfo info{};
if (SDL_GetRendererInfo(renderer, &info) == 0) {
    helipad::log_info("renderer: {} (flags: 0x{:x})", info.name, info.flags);
}
```

SDL still provides C-style APIs; use designated initializers for your own `struct`s that mirror SDL state:

```cpp
struct window_create_info {
    std::string_view title;
    glm::ivec2 size {1280, 720};
    bool high_dpi {true};
};

const window_create_info info {
    .title = "space_war",
    .size = {1600, 900},
    .high_dpi = true,
};
```

## 2. `constexpr`/`constinit` Configuration

Compile-time configuration keeps runtime clean and enforces invariants.

```cpp
constexpr glm::vec2 default_viewport_size() noexcept {
    return {1280.0f, 720.0f};
}

constinit const char* k_engine_tag = "helipad";
```

Use `constexpr` to precompute lookup tables (e.g., key remaps) so they live in `.rodata` and incur no runtime cost.

## 3. `std::span` for Buffer Views

Prefer `std::span` when passing contiguous SDL buffers or vertex data to avoid copies and allow raw pointer interop.

```cpp
void submit_vertices(std::span<const helipad_vertex> vertices) {
    SDL_RenderGeometryRaw(m_sdl_renderer,
                          nullptr,
                          reinterpret_cast<const float*>(vertices.data()),
                          sizeof(helipad_vertex),
                          nullptr,
                          0,
                          vertices.size());
}
```

`std::span` is perfect for asset loaders, constant vertex buffers, or audio samples because it works with `std::vector`, `std::array`, or raw arrays without conversions.

## 4. Lightweight Callback Alternatives

Avoid `std::function` in per-frame callbacks to dodge allocations. Use templated delegates or `std::unique_ptr` to polymorphic objects only where needed.

```cpp
template <typename Fn>
class fixed_delegate {
public:
    constexpr fixed_delegate(Fn&& fn) noexcept : m_fn(static_cast<Fn&&>(fn)) {}

    template <typename... Args>
    constexpr void operator()(Args&&... args) noexcept {
        m_fn(static_cast<Args&&>(args)...);
    }

private:
    Fn m_fn;
};

using tick_delegate = fixed_delegate<void (*)(scene_context&, float)>;
```

Alternatively, accept auto template parameters in the scene API (C++20 abbreviated function templates):

```cpp
template <typename TickFn>
scene make_scene(std::string_view name, TickFn&& on_tick) {
    return scene{name, std::forward<TickFn>(on_tick)};
}
```

## 5. Explicit Error Signalling (Exception-Light)

Instead of exceptions, return `struct result { bool ok; payload data; error_code err; }` or adopt a small `expected<T, E>` helper. C++20 cannot yet use `std::expected`, but a tiny alternative is trivial.

```cpp
enum class renderer_error { none, sdl_failure, no_device };

struct renderer_result {
    renderer_error error {renderer_error::none};
    helipad::render::device device;
    static renderer_result failure(renderer_error err) noexcept { return {.error = err}; }
};

renderer_result create_device(SDL_Window* window) noexcept {
    if (SDL_Renderer* sdl = SDL_CreateRenderer(window, nullptr, SDL_RENDERER_ACCELERATED)) {
        return {.error = renderer_error::none, .device = render::device{sdl}};
    }
    return renderer_result::failure(renderer_error::sdl_failure);
}
```

At call sites:

```cpp
if (auto result = create_device(main_window.native()); result.error == renderer_error::none) {
    m_device = std::move(result.device);
} else {
    helipad::log_error("failed to create renderer: {}", sdl_last_error());
    return false;
}
```

## 6. Structured Bindings in ECS & SDL Data

Structured bindings keep systems concise without temporary structs.

```cpp
auto view = registry.view<component_transform, component_sprite>();
for (auto [entity, transform, sprite] : view.each()) {
    const auto render_position = transform.position;
    renderer.submit(make_render_item(sprite, render_position));
}
```

When polling SDL events:

```cpp
SDL_Event event;
while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN: {
        const auto& key = event.key;
        m_input_state.set_pressed(static_cast<SDL_Scancode>(key.keysym.scancode));
        break;
    }
    default: break;
    }
}
```

## 7. Concepts for Stronger Interfaces

Concepts help express intent (e.g., �any resource handle with `.value()`�), enabling generic helpers that obey the style guide.

```cpp
template <typename Handle>
concept resource_handle = requires(Handle h) {
    { h.value() } -> std::convertible_to<uint32_t>;
    { h.valid() } -> std::same_as<bool>;
};

template <resource_handle Handle>
class handle_hash {
public:
    [[nodiscard]] size_t operator()(Handle h) const noexcept { return std::hash<uint32_t>{}(h.value()); }
};
```

This keeps your unordered maps lean and avoids accidental use of unrelated types.

## 8. Ranges & Views for Data Pipelines

C++20 ranges clean up pipelines like asset discovery or command queues without building temporary vectors.

```cpp
namespace fs = std::filesystem;

auto shader_files = fs::directory_iterator{root} | std::views::filter([](const auto& entry) {
    return entry.is_regular_file() && entry.path().extension() == ".glsl";
});

for (const auto& entry : shader_files) {
    compile_shader(entry.path());
}
```

Be mindful that ranges can introduce tiny overhead; for per-frame hot paths prefer simple loops. Use ranges for tooling, builds, or initialization flows where clarity wins.

## 9. `std::bit_cast` and `std::endian`

SDL often describes colors or pixel formats as integers. `std::bit_cast` gives you safe reinterpretation without UB.

```cpp
struct rgba { std::uint8_t r, g, b, a; };
const std::uint32_t packed = 0xff3366ffu;
auto color = std::bit_cast<rgba>(packed);
```

Keep this to debug/util code; for production use SDL�s own helpers or your math structs to avoid endian pitfalls.

## 10. `std::chrono` + `std::stop_token`

C++20�s time utilities integrate nicely with SDL high-resolution timers and keep units clear.

```cpp
using clock = std::chrono::steady_clock;

const auto now = clock::now();
const auto frame_delta = now - m_last_frame;
m_last_frame = now;

ctx.delta_s = std::chrono::duration<float>(frame_delta).count();
```

For worker threads (asset streaming, hot reload watchers), use `std::jthread` and `std::stop_token` for RAII cancellation.

```cpp
std::jthread watcher {[this](std::stop_token stop) {
    while (!stop.stop_requested()) {
        poll_file_changes();
        std::this_thread::sleep_for(100ms);
    }
}};
```

## 11. `std::optional` & `std::array` in Place of Pointers

Instead of raw pointers that may be null, use `std::optional` for state that can be absent, especially in hot loops where branch prediction matters.

```cpp
struct camera_state {
    std::optional<glm::vec2> follow_position;
    float zoom {1.0f};
};

if (camera.follow_position) {
    camera.position = glm::mix(camera.position, *camera.follow_position, 0.1f);
}
```

`std::array` gives stack storage without heap allocations�ideal for fixed-size controller states or render queues when the size is known.

## 12. Formatting (`std::format`) vs Logging

`std::format` is convenient but can heap-allocate. For hot-path logging prefer a lightweight formatter (e.g., {fmt} with pre-reserved buffers) or skip formatting entirely. Use `std::format` in tooling, or wrap your logger to preallocate per-thread buffers.

```cpp
helipad::log_debug("submitted {} sprites (layer {})", sprite_count, layer);
```

Since Helipad already wraps logging, you can swap out the backend later if `std::format` becomes a bottleneck.

## 13. `consteval` Utility IDs

When you need compile-time hashed IDs (resource handles, ECS type IDs), `consteval` gives you clarity.

```cpp
consteval std::uint32_t hash_id(std::string_view key) {
    std::uint32_t hash = 2166136261u;
    for (const char c : key) {
        hash ^= static_cast<std::uint8_t>(c);
        hash *= 16777619u;
    }
    return hash;
}

constexpr std::uint32_t sprite_resource_id = hash_id("helipad.sprite");
```

## 14. `[[likely]]` / `[[unlikely]]`

For branch-heavy SDL event loops or physics collision checks, the new attributes can coax the compiler, but use sparingly.

```cpp
if ([[likely]] (renderable.visible)) {
    submit_to_render_queue(renderable, transform);
}
```

Ensure they actually help (measure!).

## 15. Modules? Not Yet

C++20 modules are attractive but tooling and third-party libs (SDL, EnTT, glm) still lean on headers. Stick to headers for now; revisit modules once your dependency set is ready. You can modularize internal subsystems later without affecting SDL interop.

## Putting It All Together: SDL Event Pump Example

```cpp
void input_system::poll_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event) == 1) {
        switch (event.type) {
        case SDL_EVENT_KEY_DOWN: {
            const auto& key = event.key;
            const bool repeat = key.repeat != 0;
            if (!repeat) {
                set_pressed(static_cast<SDL_Scancode>(key.keysym.scancode));
            }
            break;
        }
        case SDL_EVENT_KEY_UP: {
            set_released(static_cast<SDL_Scancode>(event.key.keysym.scancode));
            break;
        }
        case SDL_EVENT_WINDOW_RESIZED: {
            const auto size = glm::ivec2{event.window.data1, event.window.data2};
            m_resize_queue.push_back(size);
            break;
        }
        default: break;
        }
    }
}
```

- Structured bindings keep field access tight.
- No exceptions thrown; errors recorded via logger.
- `glm::ivec2` remains constexpr-friendly.
- `m_resize_queue` can be a `std::vector<glm::ivec2>` or circular buffer; exposures to services happen after the loop.

## Checklist for Helipad

- [ ] Use `std::span` for renderer submissions and asset streaming APIs.
- [ ] Replace `std::function` in callbacks with delegates or templated functors.
- [ ] Introduce tiny `expected`-style structs for error propagation.
- [ ] Adopt `constexpr` tables for input mapping.
- [ ] Leverage structured bindings and concepts in ECS utilities.
- [ ] Keep the exception boundary at runtime init only.

C++20 is a sweet spot for building a modern SDL-based engine: you get clarity, type safety, and compile-time power without sacrificing determinism or bringing in heavy runtime overhead. Use these features selectively where they enhance readability and reliability, and pair them with Helipad�s style rules for a cohesive codebase.
