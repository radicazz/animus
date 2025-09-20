# Style Guide

A comprehensive style guide for consistent and maintainable code across the this game engine project.

## Table of Contents

- [General](#general)
- [C++](#c)
  - [Naming Conventions](#naming-conventions)
  - [Code Organization](#code-organization)
  - [Type Design](#type-design)
    - [Struct vs Class Rule](#struct-vs-class-rule)
    - [Class Structure Organization](#class-structure-organization)
    - [Constructor Guidelines](#constructor-guidelines)
  - [Function Design](#function-design)
  - [Modern C++ Features](#modern-c-features)
  - [Memory Management](#memory-management)
  - [Error Handling](#error-handling)
  - [Documentation](#documentation)
- [Python](#python)

---

## General

This style guide ensures consistency across the Helipad game engine project. All contributors should follow these guidelines to maintain code quality and readability.

**Key Principles:**

- **Consistency**: Follow established patterns throughout the codebase
- **Clarity**: Write self-documenting code that's easy to understand
- **Performance**: Prefer zero-cost abstractions and efficient implementations
- **Safety**: Use modern C++ features that prevent common bugs

---

## C++

The C++ style follows STL conventions with snake_case naming and modern C++20 features.

### Naming Conventions

#### Variables and Functions

Use `snake_case` for all variables, functions, and namespaces:

```cpp
// ✅ Good
int frame_count = 0;
void process_events();
namespace engine { }

// ❌ Bad  
int frameCount = 0;
void ProcessEvents();
namespace Engine { }
```

#### Classes and Structs

Use `snake_case` with descriptive prefixes for engine classes:

```cpp
// ✅ Good - Engine classes with prefixes
class game_engine;
struct game_color;
struct component_transform;

// ✅ Good - Simple structs
struct point;
struct rectangle;

// ❌ Bad
class GameEngine;
struct GameColor;
```

#### Template Parameters

Use `PascalCase` for template parameters:

```cpp
// ✅ Good
template <class T, typename ValueType>
class container;

// ❌ Bad
template <class t, typename value_type>
class container;
```

#### Constants and Enumerations

Use `snake_case` for constants and enum values. **Always use `enum class`** instead of plain `enum` for type safety:

```cpp
// ✅ Good
constexpr float default_tick_rate = 32.0f;
constexpr game_color transparent = {0, 0, 0, 0};

enum class input_state {
    pressed,
    released,
    held
};

enum class render_layer {
    background = 0,
    entities = 10,
    ui = 20
};

// ❌ Bad
constexpr float DEFAULT_TICK_RATE = 32.0f;
#define DEFAULT_TICK_RATE 32.0f

enum input_state {  // Plain enum - avoid
    PRESSED,
    RELEASED,
    HELD
};
```

#### Member Variables

Use `m_` prefix for private/protected member variables:

```cpp
// ✅ Good
class game_engine {
private:
    bool m_is_running;
    float m_tick_interval_seconds;
    game_window m_window;
};

// ✅ Good - Public members in structs (no prefix)
struct component_transform {
    glm::vec2 position = {0.0f, 0.0f};
    float rotation = 0.0f;
    glm::vec2 scale = {1.0f, 1.0f};
};
```

### Code Organization

#### Header Structure

Organize headers with clear sections and include guards:

```cpp
/**
 * @file filename.hxx
 * @brief Brief description of the file's purpose.
 */

#pragma once

// System includes
#include <memory>
#include <string_view>

// Third-party includes  
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

// Engine includes
#include "other_engine_header.hxx"

namespace engine {
    // Forward declarations first
    class game_engine;
    
    // Type definitions
    struct game_info {
        // Implementation
    };
    
    // Main class definitions
    class game_engine {
        // Implementation
    };
    
    // Inline function definitions
    inline void game_engine::simple_function() {
        // Simple implementations only
    }
}  // namespace engine
```

#### Namespace Usage

- All engine code lives in the `engine` namespace
- Use nested namespaces for specific subsystems when appropriate
- Never use `using namespace` in headers

```cpp
// ✅ Good
namespace engine {
    namespace colors {
        constexpr game_color red = {255, 0, 0, 255};
    }
}

// ✅ Good - In implementation files only
using engine::game_engine;
using engine::game_color;
```

### Type Design

#### Struct vs Class Rule

Use `struct` when all members are public, `class` when there are private members:

```cpp
// ✅ Good - Simple data with all public members
struct component_transform {
    glm::vec2 position = {0.0f, 0.0f};
    float rotation = 0.0f;
    glm::vec2 scale = {1.0f, 1.0f};
};

// ✅ Good - Complex type with private state
class game_engine {
public:
    void run();
    
private:
    bool m_is_running;
    // Other private members
};
```

#### Class Structure Organization

**Public API comes first and follows a specific order:**

1. **Constructors** (default, parameterized, copy, move)
2. **Destructor**
3. **Operator overloads** (assignment, comparison, etc.)
4. **Methods** (grouped logically)

Then **public variables** (if any) with their own `public:` section.

**Protected members** come next (same organization as public).

**Private implementation** comes last with separate sections:

1. **Private methods** (if any) with their own `private:` specifier
2. **Private variables** with their own `private:` specifier

```cpp
// ✅ Good - Proper class organization
class game_engine {
public:
    // Constructors
    game_engine() = delete;
    explicit game_engine(const game_info& info, std::string_view title, const glm::ivec2& size);
    game_engine(const game_engine&) = delete;
    game_engine(game_engine&&) = delete;
    
    // Destructor
    ~game_engine();
    
    // Operator overloads
    game_engine& operator=(const game_engine&) = delete;
    game_engine& operator=(game_engine&&) = delete;
    
    // Methods - grouped logically
    void run();
    void set_tick_rate(float tick_rate_seconds);
    
    // Accessors
    [[nodiscard]] game_window& get_window();
    [[nodiscard]] game_renderer& get_renderer();
    [[nodiscard]] float get_tick_rate();

public:
    // Public variables (if any - rare in classes)
    // Usually prefer private with accessors

protected:
    // Protected methods (if any)
    void process_events();
    
protected:
    // Protected variables (if any)
    bool m_is_running;

private:
    // Private methods (if any)
    void initialize_systems();
    void cleanup_resources();

private:
    // Private variables
    game_info m_game;
    game_window m_window;
    game_renderer m_renderer;
    float m_tick_interval_seconds;
};

// ✅ Good - Struct with simple public-only layout
struct game_scene_info {
    // All members public, no access specifiers needed
    std::string scene_id;
    scene_state state = scene_state::unloaded;
    void* scene_state = nullptr;
    
    // Methods can be mixed with data in structs
    template <class T>
        requires std::is_class_v<T>
    [[nodiscard]] T& get_state();
};
```

#### Constructor Guidelines

Use appropriate constructor patterns:

```cpp
// ✅ Good - Explicit single-argument constructors
struct game_color {
    constexpr game_color();
    constexpr game_color(int red, int green, int blue, int alpha = 255);
    constexpr explicit game_color(std::uint32_t hex_color);
};

// ✅ Good - Delete unwanted operations
class game_engine {
public:
    game_engine() = delete;
    game_engine(const game_engine&) = delete;
    game_engine& operator=(const game_engine&) = delete;
    game_engine(game_engine&&) = delete;
    game_engine& operator=(game_engine&&) = delete;
};
```

### Function Design

#### Const Correctness

Apply `const` correctly throughout:

```cpp
// ✅ Good - Const methods and parameters
class game_color {
public:
    [[nodiscard]] constexpr bool is_transparent() const;
    [[nodiscard]] SDL_Color to_sdl_color() const;
    static game_color from_sdl_color(const SDL_Color& sdl_color);
};

// ✅ Good - Const references for parameters
void render_sprite(const sprite& spr, const glm::vec2& position);
```

**Const Parameters in Declarations vs Definitions:**

For by-value parameters, **omit `const` in declarations** but **include `const` in definitions** when the parameter won't be modified:

```cpp
// ✅ Good - Header declaration (no const for by-value parameters)
void set_position(float x, float y);
void set_color(game_color color);
int calculate_distance(glm::vec2 point1, glm::vec2 point2);

// ✅ Good - Implementation (const for by-value parameters that won't change)
void sprite::set_position(const float x, const float y) {
    m_position = {x, y};  // x and y are not modified
}

void entity::set_color(const game_color color) {
    m_color = color;  // color is not modified
}

int math::calculate_distance(const glm::vec2 point1, const glm::vec2 point2) {
    return glm::distance(point1, point2);  // points are not modified
}

// ✅ Good - References and pointers are const in both declaration and definition
void render_sprite(const sprite& spr, const glm::vec2& position);
void process_entity(const game_entity* entity);
```

**Rationale:** `const` on by-value parameters is an implementation detail that doesn't affect the caller and clutters the interface. It belongs in the implementation where it helps prevent accidental modification.

#### Return Value Guidelines

Use appropriate return patterns:

```cpp
// ✅ Good - [[nodiscard]] for important return values
[[nodiscard]] game_window& get_window();
[[nodiscard]] bool is_running() const;
[[nodiscard]] constexpr bool is_transparent() const;

// ✅ Good - References for engine subsystems
game_renderer& get_renderer();
const game_input& get_input() const;
```

#### Parameter Passing

Follow modern C++ parameter guidelines:

```cpp
// ✅ Good - Various parameter types
void set_title(std::string_view title);           // String views for string parameters
void set_position(const glm::vec2& pos);          // Const ref for non-trivial types
void set_visible(bool visible);                   // Value for trivial types
void process_callback(void (*callback)(game_engine*)); // Function pointers as-is
```

### Modern C++ Features

#### Prefer constexpr over Macros

Use `constexpr` and `consteval` instead of preprocessor macros:

```cpp
// ✅ Good
constexpr float default_tick_rate = 32.0f;
constexpr game_color red = {255, 0, 0, 255};

constexpr float ticks_rate_to_interval(float rate) {
    return 1.0f / rate;
}

// ❌ Bad
#define DEFAULT_TICK_RATE 32.0f
#define RED_COLOR {255, 0, 0, 255}
```

#### Use Attributes Appropriately

Apply standard attributes for better code safety:

```cpp
// ✅ Good - [[nodiscard]] for important returns
[[nodiscard]] bool initialize();
[[nodiscard]] std::optional<sprite> load_sprite(std::string_view path);

// ✅ Good - [[maybe_unused]] for conditional parameters  
void debug_log([[maybe_unused]] std::string_view message) {
#ifdef DEBUG
    std::cout << message << '\n';
#endif
}
```

#### Type Casting

**Always use C++ style casts** instead of C-style casts for type safety and clarity:

```cpp
// ✅ Good - C++ style casts
auto value = static_cast<float>(integer_value);
auto texture = reinterpret_cast<SDL_Texture*>(void_pointer);
auto derived = dynamic_cast<derived_class*>(base_pointer);
const auto& readonly = const_cast<const game_object&>(mutable_object);

// ❌ Bad - C-style casts
auto value = (float)integer_value;
auto texture = (SDL_Texture*)void_pointer;
auto derived = (derived_class*)base_pointer;
```

**Cast Selection Guidelines:**

- `static_cast<T>()` - Most common, for safe conversions
- `reinterpret_cast<T>()` - Low-level pointer/reference conversions
- `dynamic_cast<T>()` - Polymorphic type conversions with runtime checking
- `const_cast<T>()` - Adding/removing const (use sparingly)

#### Auto Usage Guidelines

Use `auto` judiciously in specific scenarios only. **Do not use auto for regular function return values.**

**✅ Acceptable auto usage:**

```cpp
// 1. When assigned value has explicit cast (type is clear)
auto radius = static_cast<float>(diameter / 2);
auto count = static_cast<std::size_t>(entities.size());

// 2. Obnoxiously long types
auto texture = std::make_unique<SDL_Texture, decltype(&SDL_DestroyTexture)>(
    SDL_CreateTexture(renderer, format, access, width, height), 
    &SDL_DestroyTexture
);

auto iterator = std::unordered_map<std::string, std::unique_ptr<game_resource>>::const_iterator{};

// 3. Structured bindings
auto [position, rotation, scale] = get_transform_components();
auto [width, height] = window.get_size();

// 4. Range-based for loops
for (const auto& entity : entities) {
    // Process entity
}

for (auto& [name, resource] : resource_map) {
    // Process name and resource
}
```

**❌ Avoid auto for:**

```cpp
// ❌ Bad - Regular function returns (type unclear)
auto window = get_window();              // Use: game_window& window = get_window();
auto result = calculate_distance(a, b);  // Use: float result = calculate_distance(a, b);
auto entities = get_entities();          // Use: game_entities& entities = get_entities();

// ❌ Bad - Simple assignments without casts
auto count = 42;          // Use: int count = 42;
auto name = "player";     // Use: const char* name = "player"; or std::string_view
auto position = {0, 0};   // Use: glm::vec2 position = {0, 0};
```

#### Trailing Return Types

Use trailing return types (`auto function() -> return_type`) **only** to resolve namespace ambiguity:

```cpp
// ✅ Good - Resolving namespace ambiguity
auto some::long::namespace_name::create_entity() -> entity_type {
    return entity_type{};
}

auto graphics::renderer::create_texture() -> texture_handle {
    return texture_handle{};  
}

// ✅ Good - Regular function (no trailing return needed)
game_entity create_player() {
    return game_entity{};
}

// ❌ Bad - Unnecessary trailing return
auto get_position() -> glm::vec2 {  // Just use: glm::vec2 get_position()
    return glm::vec2{0, 0};
}
```

#### Template Constraints

Use concepts and requires clauses for template constraints:

```cpp
// ✅ Good - Clear template constraints
template <class T>
    requires std::is_class_v<T>
[[nodiscard]] T& get_state();

template <typename T>
concept Component = requires {
    typename T::component_tag;
};

template <Component T>
void add_component(entity_id id, const T& component);
```

#### Smart Pointers

Use appropriate smart pointer types:

```cpp
// ✅ Good - unique_ptr for owned resources
std::unique_ptr<SDL_Texture, SDL_DestroyTexture> texture;

// ✅ Good - shared_ptr for shared ownership
std::shared_ptr<font_resource> default_font;

// ✅ Good - Raw pointers for non-owning references
void render(const sprite* spr);  // Non-owning, can be null
void render(const sprite& spr);  // Non-owning, never null
```

#### References vs Pointers

**Prioritize references over raw pointers** when `nullptr` is never a possibility:

```cpp
// ✅ Good - Reference when null is impossible
game_entity& get_player();                    // Always returns valid entity
void update_entity(game_entity& entity);      // Entity must exist
const texture& get_default_texture();         // Always has default

// ✅ Good - Pointer when null is possible  
game_entity* find_entity_by_name(std::string_view name);  // May not exist
sprite* get_sprite_if_loaded(texture_id id);             // May not be loaded
const font* try_get_font(std::string_view name);         // May fail

// ❌ Bad - Pointer when null is impossible
game_entity* get_player();        // Use reference - player always exists
void update(game_entity* entity);  // Use reference - entity must be valid
```

#### Memory Allocation

**Never use `new`/`delete` outside of constructors and destructors:**

```cpp
// ✅ Good - new/delete only in special member functions
class resource_manager {
public:
    resource_manager() : m_buffer(new char[buffer_size]) {}  // OK in constructor
    ~resource_manager() { delete[] m_buffer; }               // OK in destructor
    
private:
    char* m_buffer;
};

// ✅ Good - Use smart pointers or containers instead
class game_world {
    std::vector<game_entity> entities;           // Automatic memory management
    std::unique_ptr<physics_world> physics;      // Smart pointer
    std::array<texture, max_textures> textures;  // Stack allocation
};

// ❌ Bad - raw new/delete in regular functions
void some_function() {
    auto* entity = new game_entity{};  // Bad: use std::make_unique or container
    // ... work with entity
    delete entity;                     // Bad: error-prone manual cleanup
}

// ✅ Good - Alternative approaches
void some_function() {
    // Stack allocation for temporary objects
    game_entity entity{};
    
    // Or smart pointer for dynamic allocation
    auto entity_ptr = std::make_unique<game_entity>();
    
    // Or add to container
    entities.emplace_back();
}
```

### Memory Management

#### RAII Principles

Use RAII for all resource management:

```cpp
// ✅ Good - RAII wrapper
class sdl_texture {
public:
    explicit sdl_texture(SDL_Texture* texture) : m_texture(texture) {}
    ~sdl_texture() { SDL_DestroyTexture(m_texture); }
    
    // Non-copyable, movable
    sdl_texture(const sdl_texture&) = delete;
    sdl_texture& operator=(const sdl_texture&) = delete;
    sdl_texture(sdl_texture&&) noexcept;
    sdl_texture& operator=(sdl_texture&&) noexcept;
    
private:
    SDL_Texture* m_texture;
};
```

#### Initialization

Use uniform initialization and member initializer lists:

```cpp
// ✅ Good - Uniform initialization
game_color transparent{0, 0, 0, 0};
std::vector<entity_id> entities{1, 2, 3, 4, 5};

// ✅ Good - Member initialization
game_engine::game_engine(const game_info& info, std::string_view title, const glm::ivec2& size)
    : m_is_running{true},
      m_game{info},
      m_window{title, size},
      m_tick_interval_seconds{1.0f / 32.0f} {
}
```

### Error Handling

#### Exception Safety

Write exception-safe code with proper error handling:

```cpp
// ✅ Good - Use exceptions for initialization errors
class game_engine {
public:
    game_engine(const game_info& info, std::string_view title, const glm::ivec2& size) {
        if (!initialize_sdl()) {
            throw std::runtime_error("Failed to initialize SDL");
        }
        // Other initialization...
    }
};

// ✅ Good - Optional for optional operations  
[[nodiscard]] std::optional<texture_id> load_texture(std::string_view path);

// ✅ Good - Expected for operations that can fail
[[nodiscard]] std::expected<sprite, load_error> load_sprite(std::string_view path);
```

#### Noexcept Specification

Use `noexcept` in two specific scenarios:

1. **When confident no exception will be thrown** - Functions that are inherently exception-safe
2. **When termination is preferred over exception handling** - Critical operations where failure should terminate

```cpp
// ✅ Good - Confident no exception will be thrown
class vector2 {
public:
    float x() const noexcept { return x_; }
    float y() const noexcept { return y_; }
    
    // Simple operations that cannot fail
    vector2 operator+(const vector2& other) const noexcept {
        return {x_ + other.x_, y_ + other.y_};
    }
    
    // Move operations (should never throw)
    vector2(vector2&& other) noexcept : x_(other.x_), y_(other.y_) {}
    vector2& operator=(vector2&& other) noexcept {
        x_ = other.x_;
        y_ = other.y_;
        return *this;
    }
};

// ✅ Good - Termination preferred over exception handling
class critical_system {
public:
    // Critical cleanup - better to terminate than handle exceptions
    ~critical_system() noexcept {
        cleanup_resources(); // If this fails, we want to terminate
    }
    
    // Emergency shutdown - failure means undefined state
    void emergency_shutdown() noexcept {
        // Better to terminate than continue in bad state
        force_cleanup();
    }
};

// ❌ Avoid - Don't use noexcept if function might need to throw
void load_config(std::string_view path) noexcept {  // Bad: file operations can fail
    // What if file doesn't exist? Can't throw, must terminate
}
```

### Documentation

#### Doxygen Comments

Use consistent Doxygen documentation:

```cpp
/**
 * @brief Brief description of the class or function.
 *
 * Detailed description providing context, usage examples, and important notes.
 * Multiple paragraphs are allowed and encouraged for complex functionality.
 *
 * @tparam T Template parameter description
 * @param parameter_name Description of the parameter and its constraints
 * @return Description of return value and its meaning
 * @throws exception_type When this exception is thrown
 * 
 * @note Important implementation details or usage notes
 * @warning Critical information about potential issues
 * 
 * @code
 * // Usage example
 * game_engine engine{info, "My Game", {800, 600}};
 * engine.run();
 * @endcode
 */
template <class T>
    requires std::is_class_v<T>
[[nodiscard]] T& get_state();
```

#### Comment Style

Use clear, concise comments for complex logic:

```cpp
// ✅ Good - Explain the why, not the what
// Calculate interpolated position for smooth animation between ticks
const float alpha = get_fraction_to_next_tick();
const glm::vec2 smooth_pos = glm::mix(prev_pos, curr_pos, alpha);

// ✅ Good - Document non-obvious behavior  
// SDL requires texture format conversion for optimal blending
SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
```

---

## Python

*Python style guide section will be added as the project's Python components are developed.*

**Placeholder for Python guidelines:**

- PEP 8 compliance
- Type hints usage
- Documentation standards
- Testing patterns

---

## Conclusion

This style guide ensures consistent, maintainable, and safe C++ code throughout the Helipad game engine. When in doubt, follow the existing patterns in the codebase and prioritize clarity and safety over cleverness.
