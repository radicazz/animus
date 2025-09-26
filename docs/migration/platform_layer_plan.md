# SDL3 Platform Layer Implementation Plan

> This document details the first layer of the Helipad architecture (see docs/migration/architecture_layers.md), defining a modern C++20 wrapper around SDL3 that higher layers can depend on without ever including SDL headers. It consolidates guidelines from cpp20_patterns.md, renderer_modularization_plan.md, and resource_management.md into an actionable design.

## Layer Scope

- Implements helipad::platform: application bootstrap, window management, rendering device access, timing primitives, input pumping, and low-level resource loading (textures, fonts, audio hooks).
- Owns every direct call to SDL3, SDL_image, SDL_ttf, and future SDL_* modules.
- Guarantees ABI-stable value types and handles for consumers in helipad::render, helipad::io, and helipad::services.
- Exposes no inheritance, virtual functions, or raw SDL pointers across the boundary.

## Goals & Non-Negotiables

- Keep the abstractions trivial to inline: thin structs, free functions, [[nodiscard]] factory helpers, noexcept moves.
- Zero SDL headers outside this layer; provide forward declared opaque handles or value wrappers only.
- Prefer compile-time safety: constexpr defaults, consteval id helpers, concept-gated APIs, and static checking of flag combinations.
- Explicit ownership: RAII for every SDL resource, deterministic destruction order, and visible move semantics.
- Exception-light: allow exceptions only in initialization paths; provide expected-style return types for steady-state code.
- Debug diagnostics must compile out cleanly when ENGINE_BUILD_RELEASE (or similar) is active.

## Directory Proposal

```
src/
  platform/
    core/                 // application, config handshake, subsystem guards
    window/               // window, display, monitor DPI utilities
    renderer/             // renderer_device, swap chain, surface upload helpers
    input/                // SDL event pump, controller and keyboard state capture
    resources/            // texture/font/audio loaders, file IO helpers
    diagnostics/          // assert macros, logging adapters, build flags
    detail/               // internal helpers (deleters, enums, conversions)
```

- Co-locate a new include/helipad/platform/ if we split headers for public consumption later; for now src/platform keeps headers + cxx since everything is internal.
- Alternate: embed under src/engine/platform/; rejected because higher layers (e.g. helipad::services) must depend on platform without circularity.

## Component Blueprint

### core::application

- Responsibilities: initialize SDL subsystems, configure runtime (locale, sensors), manage library shutdown.
- API sketch:

  ```cpp
  namespace helipad::platform::core {
      class application {
      public:
          struct options {
              bool video = true;
              bool audio = true;
              bool controller = true;
              bool events = true;
          };
          explicit application(options opts = {});
          application(application&&) noexcept = default;
          ~application();
          [[nodiscard]] static result<version_info> query_versions() noexcept;
      };
  }
  ```

- Throws platform_error only if initial SDL init fails; consumers catch once at boot and report.
- Alternative considered: free init()/shutdown() functions; rejected to avoid global state and enable deterministic destruction via RAII.

### window::window and window::display

- Provide strongly-typed descriptors:

  ```cpp
  struct window_create_info {
      std::string_view title;
      glm::ivec2 client_size {1280, 720};
      display_id display {display_id::primary()};
      window_flags flags;
      std::optional<float> dpi_scale_hint;
  };
  ```

- window_flags is a enum class with constexpr bitwise operators checked via static_assert.
- Methods cover: resize, show/hide, fullscreen toggle, set_icon(span<const std::byte>), clipboard access.
- Expose native_handle() guarded under HELIPAD_PLATFORM_EXPOSE_NATIVE compile flag for rare cases; default build omits it.
- Alternative: SDL_Window* shared_ptr; rejected because pointer sharing leaks SDL semantics upward.

### renderer::renderer_device

- Thin facade over SDL_Renderer or SDL_GPU once available.
- API returns POD command structs (aligns with renderer_modularization_plan.md):

  ```cpp
  struct render_frame {
      glm::ivec2 output_size;
      float dpi_scale;
  };

  class renderer_device {
  public:
      struct create_info { window& owner; renderer_backend backend; };
      static expected<renderer_device, device_error> create(const create_info& info) noexcept;
      render_frame begin_frame(clear_color color) noexcept;
      void submit(const sprite_draw& cmd) noexcept;
      void submit(const text_draw& cmd) noexcept;
      void end_frame() noexcept;
      texture_handle upload_texture(surface_view pixels, texture_params params);
      void destroy(texture_handle handle) noexcept;
  };
  ```

- sprite_draw and text_draw live in renderer/detail matching queue structures defined in higher layers.
- Implementation uses resource_management.md patterns: RAII SDL_Texture via unique_resource<T, deleter>.
- Encourage stateless operations; no hidden allocations during submit.

### resources::image_loader, texture, font

- surface_view: non-owning std::span<const std::byte> + metadata for pixel format; avoids copying raw buffers.
- texture:

  ```cpp
  class texture {
  public:
      static expected<texture, texture_error> from_file(std::string_view path, renderer_device& device) noexcept;
      static expected<texture, texture_error> from_pixels(surface_view pixels, texture_params params, renderer_device& device) noexcept;
      texture(texture&&) noexcept;
      ~texture();
      [[nodiscard]] texture_id id() const noexcept;
  };
  ```

- Fonts mirror the same approach; store metrics separately to avoid SDL_ttf lookups in hot paths.
- Alternative considered: lazy upload by returning lambdas; rejected to keep lifetime deterministic and avoid hidden heap allocations.

### input::event_pump

- Provides non-owning view of SDL events. Responsibilities: pump queue, transform into input_snapshot consumed by services layer.
- event_pump::poll(std::span<event_sink>) returns event_batch struct (counts, optional quit event).
- Use [[likely]] attributes for common event paths (keyboard/mouse) but guard with microbenchmarks.

### timing::clock

- Wraps SDL performance counters but exposes chrono-friendly API.
- frame_clock::tick() returns frame_delta struct with seconds, perf counts, smoothed average.
- Alternative: rely on std::chrono::steady_clock only; retained as fallback for testing when SDL not initialized.

### diagnostics::assertion

- Provide macros:

  ```cpp
  #if HELIPAD_DEBUG_ASSERTS
  #define HELIPAD_ASSERT(expr, ...) ::helipad::platform::diagnostics::assert_or_panic( \
        (expr), ::std::source_location::current(), ##__VA_ARGS__)
  #else
  #define HELIPAD_ASSERT(expr, ...) static_cast<void>(0)
  #endif
  ```

- assert_or_panic logs once, then either breaks into debugger or throws platform_error depending on build knob.
- Integrate with existing engine::ensure by forwarding to these macros to avoid duplication.

## Error Handling and Result Types

- Introduce lightweight template<typename T, typename E> class result located in platform/core/result.hxx:
  - constexpr constructors, no dynamic allocation, [[nodiscard]].
  - E defaults to platform_error (small enum + message span).
  - Provide to_string(E) for logging without std::string allocations (use std::string_view constants).
- Exceptions permitted only in constructors that set up SDL subsystems or allocate OS resources once. Document each throw site.
- For steady-state functions: return result or std::optional and log via diagnostics. This matches cpp20_patterns.md guidance.

## Compile-Time Controls

- CMake options:
  - HELIPAD_PLATFORM_STRICT_ASSERTS (default ON in debug).
  - HELIPAD_PLATFORM_LEAN to compile out optional subsystems (e.g., audio).
  - HELIPAD_PLATFORM_ENABLE_GPU toggles experimental SDL_GPU backend.
- Generate config.hxx entries to expose constexpr bool platform_has_audio etc.
- Use if constexpr to compile out unused code paths, particularly for controller support or IME handling.

## Integration Contracts

- helipad::render consumes only stable handles (texture_id, font_id) plus renderer_device reference for uploads (see resource_management.md).
- helipad::io obtains input_snapshot built by event_pump.
- helipad::services orchestrates caches using result to propagate load failures up to runtime.
- Provide adapter header platform/fwd.hxx exporting alias types so higher layers can forward declare without touching heavy headers.

## Testing Strategy

- Recommend doctest:
  - Header-only, compiles fast, supports BDD/CHECK macros similar to Catch2, integrates easily with SDL init fixtures.
  - Allows writing tests inside implementation files without separate translation units, aligning with "modern, easy & fast".
- Alternatives:
  1. Catch2: richer BDD syntax but slower compile times; heavier for frequent TDD on small SDL wrappers.
  2. GoogleTest: robust fixtures and mocking but requires linking libraries and increases build complexity; overkill for thin value types.
- Implementation:
  - Add tests/platform/ with doctest-based suites verifying window creation (using headless renderer flag), result propagation, RAII semantics, and error paths.
  - Provide fake SDL backend layer for CI (compile-time flag HELIPAD_PLATFORM_FAKE_SDL hooking into shim functions).

## Implementation Roadmap

1. Foundation (Week 1)
   - Add src/platform/core/application.* and diagnostics macros.
   - Introduce platform::result and platform_error.
   - Wire CMake options and generated config entries.
2. Window & Renderer Device (Week 2)
   - Implement window creation, DPI, fullscreen toggles with exhaustive tests.
   - Create renderer_device with sprite/text submission stubs, bridging into existing game_renderer via adapter.
3. Resource Wrappers (Week 3)
   - Add texture/font RAII types and integrate with resource caches (see resource_management.md).
   - Ensure zero-copy surface uploads using std::span.
4. Input & Timing (Week 4)
   - Replace current input pump with platform implementation returning neutral data structures.
   - Hook timing utilities into runtime (maintaining backwards compatibility via adapters).
5. Hardening (Week 5)
   - Audit all engine files to remove direct SDL includes.
   - Add doctest suite and CI job.
   - Document usage examples under docs/migration/ and update STYLEGUIDE.

## Tooling and Documentation

- Update docs/migration/README.md with link to this plan and checklist for migration tasks.
- Provide Doxygen comments in each public header (mirrors existing style).
- Add Clang-Tidy profile focusing on modernize-use-override disabled (no inheritance), cppcoreguidelines-pro-bounds-constant-array-index to ensure safe spans.

## Future Opportunities

- Swap backend with SDL_gpu or custom Metal/Vulkan device by implementing the same renderer_device interface.
- Extend event_pump with platform message filters for editor tooling.
- Add optional job system integration (std::jthread) for async resource loading once platform layer exposes thread utilities.
- Build headless mode (HELIPAD_PLATFORM_NO_WINDOW) for dedicated server or continuous testing.

## Exit Criteria

- No SDL includes leak beyond src/platform.
- All SDL resources are RAII-managed with move semantics verified by tests.
- Higher layers consume only handles and POD structs defined here.
- Build toggles documented and enforced via CI matrices (debug, release, lean).
- Migration checklist complete; existing demo runs unchanged with new adapters.
