#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace engine {
    class game_window {
    public:
        explicit game_window(std::string_view title, const glm::ivec2& size);
        ~game_window();

        // Resource management - disable copy, enable move
        game_window(const game_window&) = delete;
        game_window& operator=(const game_window&) = delete;
        game_window(game_window&& other) noexcept;
        game_window& operator=(game_window&& other) noexcept;

        [[nodiscard]] SDL_Window* get_sdl_window() const;

        [[nodiscard]] std::string get_title() const;
        void set_title(std::string_view new_title);

        [[nodiscard]] glm::ivec2 get_logical_size() const;
        void set_logical_size(const glm::ivec2& size);

        [[nodiscard]] glm::ivec2 get_pixel_size() const;

        void set_icon(std::string_view icon_path);

    private:
        SDL_Window* m_window;
    };

    inline SDL_Window* game_window::get_sdl_window() const {
        return m_window;
    }
}  // namespace engine
