#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace engine {
    class game_window {
    public:
        game_window(std::string_view title, int width, int height);
        ~game_window();

        [[nodiscard]] SDL_Window* get_sdl_window() const;

        [[nodiscard]] std::string get_title() const;
        void set_title(std::string_view new_title);

        [[nodiscard]] glm::vec2 get_logical_size() const;
        [[nodiscard]] glm::vec2 get_pixel_size() const;

        [[nodiscard]] bool get_is_running() const;

        /**
         * @brief Set the running state of the window.
         * @param is_running The new running state.
         * @note Setting this to false will close the window.
         */
        void set_is_running(bool is_running);

    private:
        SDL_Window* m_window;
        bool m_is_running;
    };

    inline SDL_Window* game_window::get_sdl_window() const {
        return m_window;
    }

    inline bool game_window::get_is_running() const {
        return m_is_running;
    }

    inline void game_window::set_is_running(bool is_running) {
        m_is_running = is_running;
    }

}  // namespace engine
