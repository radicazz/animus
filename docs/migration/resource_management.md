# Resource Management & SDL Wrappers

> Design notes for handling textures, fonts, and other SDL-backed assets in Helipad�s layered architecture.

## Goals

- **Safe lifetimes**: SDL objects are always destroyed exactly once, even during hot reloads.
- **Layer isolation**: engine and gameplay code never includes SDL headers.
- **Deterministic handles**: gameplay logic references resources via lightweight IDs that survive reloads.
- **Extensibility**: flexible enough to add asynchronous streaming, packing, or alternate render backends later.

## Layer Responsibilities

```
platform (SDL)
  +-- detail wrappers (SDL_Texture*, TTF_Font*)
render/device layer
  +-- texture_cache, font_cache (create/destroy, handle registry)
services/resource_manager
  +-- gameplay-facing API (load_sprite, load_font, retain/release)
engine/scenes/ECS
  +-- store handles in components; never touch SDL
```

- **`helipad::platform`**: owns all direct SDL calls. Provides RAII wrappers (`unique_texture`, `unique_font`), pixel loaders (stb_image), and device constructors.
- **`helipad::render`**: wraps platform objects into caches and exposes `texture_handle`, `font_handle`, `text_handle` types.
- **`helipad::services`**: a `resource_manager` that orchestrates caches, reference counts, hot reload hooks, and future streaming.
- **`helipad::engine`**: systems/components keep immutable metadata (`sprite_resource`, `text_resource`) plus instance data (`sprite_instance`).

## RAII Wrappers (Platform Layer)

```cpp
namespace helipad::render::detail {
    struct texture_deleter {
        void operator()(SDL_Texture* tex) const noexcept { SDL_DestroyTexture(tex); }
    };
    using texture_ptr = std::unique_ptr<SDL_Texture, texture_deleter>;

    struct font_deleter {
        void operator()(TTF_Font* font) const noexcept { TTF_CloseFont(font); }
    };
    using font_ptr = std::unique_ptr<TTF_Font, font_deleter>;
}
```

- RAII objects live only in the platform/back-end code.
- No upper layer ever sees raw SDL pointers.

## Strong Handles

```cpp
template <typename Tag>
class handle {
public:
    static handle invalid() { return handle{}; }
    bool valid() const noexcept { return m_value != invalid_value; }
    uint32_t value() const noexcept { return m_value; }

private:
    explicit handle(uint32_t value) : m_value(value) {}
    uint32_t m_value = invalid_value;
    static constexpr uint32_t invalid_value = std::numeric_limits<uint32_t>::max();
};

using texture_handle = handle<struct texture_tag>;
using font_handle    = handle<struct font_tag>;
using text_handle    = handle<struct text_tag>;
```

- Handles are small, copyable, and hashable.
- They survive asset reloads because the cache can replace the underlying SDL object without changing the handle.
- Optional: wrap in a smart-handle class that automatically `retain()`/`release()` when copied/moved.

## Texture Cache

```cpp
class texture_cache {
public:
    explicit texture_cache(render::device& device);

    texture_handle load_sprite(std::string_view key, std::string_view file_path);
    void retain(texture_handle h);
    void release(texture_handle h);

    SDL_Texture* native(texture_handle h) const noexcept;

private:
    struct entry {
        render::detail::texture_ptr texture;
        uint32_t pins = 1;
        std::string key;
    };

    std::unordered_map<std::string, texture_handle> m_lookup;
    std::vector<entry> m_entries;
    render::device* m_device {nullptr};
};
```

- `load_sprite`: returns existing handle if already loaded; otherwise decodes PNG (via stb_image or SDL_Image) and uploads via `render::device`.
- `retain` / `release`: manage pin counts. When `pins == 0`, remove the entry and RAII wrapper frees the SDL texture.
- `native(handle)`: used only by render device to issue draw calls. Rest of the engine sticks to handles.

### Loading Flow

1. Gameplay code calls `resource_manager.load_sprite("player", path)`.
2. Resource manager forwards to texture cache.
3. Cache loads image data -> `render::device::create_texture(data)` -> stores RAII pointer and returns handle.
4. Scene/ECS store the handle.

### Unloading Flow

- When scene unloads: call `release(handle)` for each pinned resource.
- For automatic lifetime, prefer a smart handle:

```cpp
class sprite_ref {
public:
    sprite_ref(texture_cache& cache, texture_handle handle)
        : m_cache(&cache), m_handle(handle) {}
    ~sprite_ref() { if (m_handle.valid()) m_cache->release(m_handle); }
    sprite_ref(sprite_ref&& other) noexcept { swap(other); }
    sprite_ref& operator=(sprite_ref&& other) noexcept { swap(other); return *this; }
private:
    void swap(sprite_ref& other) noexcept { std::swap(m_cache, other.m_cache); std::swap(m_handle, other.m_handle); }
    texture_cache* m_cache{};
    texture_handle m_handle{};
};
```

## Font & Text Cache

- Fonts use the same pattern. Unique lookup key includes font path + point size + style.
- For static text (rare updates): cache an SDL texture once; reuse it like a sprite.
- For dynamic text: keep `text_handle` that stores `font_handle`, string, color, metrics. Rebuild SDL textures only when the string changes.
- Later optimization: glyph atlases to avoid regenerating full textures.

## Immutable Resources vs Instance Data

- **Resource metadata** (`sprite_resource`): file path, texture handle, default size/origin.
- **Instance data** (`sprite_instance`): transform, tint, per-entity settings. Stored in ECS components or render commands.

```cpp
struct sprite_resource {
    texture_handle texture;
    glm::vec2 size;
    glm::vec2 origin;
};

struct sprite_instance {
    sprite_resource resource;
    glm::vec2 position;
    glm::vec2 scale {1.f};
    float rotation {0.f};
    glm::vec4 tint {1.f};
};
```

> Rendering systems fetch `sprite_resource` from the cache and combine it with `sprite_instance` to submit render items. No `set_rotation`/`set_scale` calls on shared resources.

## Services Layer Facade

Expose a convenient API that scenes and gameplay code consume:

```cpp
class resource_manager {
public:
    sprite_handle load_sprite(std::string_view key, std::string_view path);
    font_handle   load_font(std::string_view key, std::string_view path, float pt_size);
    text_handle   create_text(font_handle font, std::string_view text, glm::vec4 color);

    const sprite_resource& sprite(sprite_handle h) const;
    const text_resource&   text(text_handle h) const;

    void reload_if_dirty(std::string_view key);   // hot reload hook
    void release(sprite_handle h);                // manual release if needed
};
```

- Behind the scenes it coordinates texture cache, font cache, text cache.
- Supports hot reload by invalidating/reloading specific keys or scanning the file system.
- UI/editor can inspect loaded resources via this layer for debugging.

## ECS Integration

Components keep handles only:

```cpp
struct component_sprite {
    sprite_handle resource;
    glm::vec2 scale {1.f};
    glm::vec4 tint {1.f};
};

struct component_text {
    text_handle resource;
    glm::vec2 offset {0.f};
};
```

Renderer system collects transforms + handle and emits render queue entries:

```cpp
void system_renderer::update(entt::registry& reg,
                             render_manager& renderer,
                             resource_manager& resources) {
    auto view = reg.view<component_transform, component_renderable, component_sprite>();
    for (auto [entity, transform, renderable, sprite_comp] : view.each()) {
        if (!renderable.visible) continue;
        const auto& sprite = resources.sprite(sprite_comp.resource);

        render_manager::render_item item{};
        item.texture = sprite.texture;
        item.transform = compose_matrix(transform.position,
                                         transform.rotation,
                                         sprite.size * sprite_comp.scale);
        item.tint = sprite_comp.tint;
        item.layer = renderable.layer;

        renderer.submit(item);
    }
}
```

## Hot Reload & Streaming

- **Hot reload**: resource manager watches modification timestamps. When a file changes, it reloads into the same handle (replace RAII pointer), keeping gameplay code unaffected.
- **Streaming**: jobs thread loads file + decode into CPU buffer; main thread calls `texture_cache.upload(handle, data)` to avoid touching SDL from other threads.
- **Diagnostics**: track who retained handles and print on shutdown to catch leaks.

## Testing & Tooling

- Mock texture cache by creating dummy handles pointing to fake textures.
- Editor can list all loaded resources plus their pin counts by querying `resource_manager`.
- Render tests can satisfy `native(handle)` with stub textures that validate parameters without actual SDL.

## Summary

- RAII wraps SDL objects at the lowest layer.
- Strong handles + reference counting keep lifetimes explicit and safe.
- Immutable resource data separates shared assets from per-entity state.
- Services expose a clean API compatible with hot reloads, streaming, and editor workflows.
- Gameplay code remains agnostic to SDL, easing future backend swaps and scripting integrations.
