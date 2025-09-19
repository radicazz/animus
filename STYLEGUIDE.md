# Style Guide

A comprehensive style guide for consistent and maintainable code across the this game engine project.

## Table of Contents

- [General](#general)
- [C++](#c)
  - [Naming Conventions](#naming-conventions)
  - [Code Organization](#code-organization)
  - [Type Design](#type-design)
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
