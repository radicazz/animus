#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace engine {
    class window {
    public:
        window(std::string_view title, int width, int height);
        ~window();

        SDL_Window* get_sdl_window() const;

        std::string get_title() const;
        void set_title(std::string_view new_title);

        glm::vec2 get_logical_size() const;
        glm::vec2 get_pixel_size() const;

        bool is_running() const;

        /**
         * @brief Set the running state of the window.
         *
         * @note Setting this to false will close the window.
         *
         * @param is_running The new running state.
         */
        void set_is_running(bool is_running);

    private:
        SDL_Window* m_window;
        bool m_is_running;
    };

    inline SDL_Window* window::get_sdl_window() const {
        return m_window;
    }

    inline bool window::is_running() const {
        return m_is_running;
    }

    inline void window::set_is_running(bool is_running) {
        m_is_running = is_running;
    }

}  // namespace engine
