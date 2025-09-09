#pragma once

#include <cstdint>
#include <SDL3/SDL.h>

namespace engine {
    /**
     * @brief Color representation for the engine.
     *
     * Provides a consistent color interface for the engine while abstracting
     * the underlying SDL color representation. Supports RGBA values with
     * convenient constructors and conversion utilities.
     */
    struct game_color {
        constexpr game_color();
        constexpr game_color(int red, int green, int blue, int alpha = 255);
        constexpr explicit game_color(float red, float green, float blue, float alpha = 1.0f);
        constexpr explicit game_color(std::uint32_t hex_color);

        constexpr bool operator==(const game_color& other) const;
        constexpr bool operator!=(const game_color& other) const;

        /**
         * @brief Get this object as an SDL_Color.
         * @return `SDL_Color` A copy of this object converted.
         * @note This utility should only be used internally within the game engine.
         */
        [[nodiscard]] SDL_Color to_sdl_color() const;

        /**
         * @brief Create a game_color from an SDL_Color.
         * @param sdl_color The SDL_Color to convert.
         * @return A game_color representing the same color as the SDL_Color.
         * @note This utility should only be used internally within the game engine.
         */
        static game_color from_sdl_color(const SDL_Color& sdl_color);

        [[nodiscard]] constexpr bool is_transparent() const;
        [[nodiscard]] constexpr bool is_opaque() const;

        std::uint8_t r, g, b, a;
    };

    inline constexpr game_color::game_color() : r(0), g(0), b(0), a(0) {
    }

    inline constexpr game_color::game_color(int red, int green, int blue, int alpha)
        : r(static_cast<std::uint8_t>(red)),
          g(static_cast<std::uint8_t>(green)),
          b(static_cast<std::uint8_t>(blue)),
          a(static_cast<std::uint8_t>(alpha)) {
    }

    inline constexpr game_color::game_color(float red, float green, float blue, float alpha)
        : r(static_cast<std::uint8_t>(red * 255.0f)),
          g(static_cast<std::uint8_t>(green * 255.0f)),
          b(static_cast<std::uint8_t>(blue * 255.0f)),
          a(static_cast<std::uint8_t>(alpha * 255.0f)) {
    }

    inline constexpr game_color::game_color(std::uint32_t hex_color) {
        if (hex_color <= 0xFFFFFF) {
            // RGB format (0xRRGGBB)
            r = static_cast<std::uint8_t>((hex_color >> 16) & 0xFF);
            g = static_cast<std::uint8_t>((hex_color >> 8) & 0xFF);
            b = static_cast<std::uint8_t>(hex_color & 0xFF);
            a = 255;
        } else {
            // RGBA format (0xRRGGBBAA)
            r = static_cast<std::uint8_t>((hex_color >> 24) & 0xFF);
            g = static_cast<std::uint8_t>((hex_color >> 16) & 0xFF);
            b = static_cast<std::uint8_t>((hex_color >> 8) & 0xFF);
            a = static_cast<std::uint8_t>(hex_color & 0xFF);
        }
    }

    inline constexpr bool game_color::operator==(const game_color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    inline constexpr bool game_color::operator!=(const game_color& other) const {
        return !(*this == other);
    }

    inline SDL_Color game_color::to_sdl_color() const {
        return SDL_Color{r, g, b, a};
    }

    inline game_color game_color::from_sdl_color(const SDL_Color& sdl_color) {
        return game_color{sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a};
    }

    inline constexpr bool game_color::is_transparent() const {
        return a == 0;
    }

    inline constexpr bool game_color::is_opaque() const {
        return a == 255;
    }

    // Some predefined colors for convenience.
    namespace colors {
        constexpr game_color transparent = {0, 0, 0, 0};
        constexpr game_color black = {0, 0, 0, 255};
        constexpr game_color white = {255, 255, 255, 255};
        constexpr game_color red = {255, 0, 0, 255};
        constexpr game_color green = {0, 255, 0, 255};
        constexpr game_color blue = {0, 0, 255, 255};
        constexpr game_color yellow = {255, 255, 0, 255};
        constexpr game_color cyan = {0, 255, 255, 255};
        constexpr game_color magenta = {255, 0, 255, 255};
        constexpr game_color orange = {255, 165, 0, 255};
        constexpr game_color purple = {128, 0, 128, 255};
        constexpr game_color gray = {128, 128, 128, 255};
        constexpr game_color light_gray = {192, 192, 192, 255};
        constexpr game_color dark_gray = {64, 64, 64, 255};
    }  // namespace colors
}  // namespace engine
