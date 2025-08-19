#pragma once

#include <string_view>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace engine {
    struct renderer_color {
        Uint8 r;
        Uint8 g;
        Uint8 b;
        Uint8 a;
    };

    class renderer {
    public:
        renderer(SDL_Window* window);
        ~renderer();

        void render_pre();
        void render_post();

        void set_draw_color(const renderer_color& color);

        void draw_rect(const glm::vec2& position, const glm::vec2& size);
        void draw_rect(const glm::vec2& position, const glm::vec2& size,
                       const renderer_color& color);

    private:
        SDL_Renderer* m_renderer;
    };

    class window {
    public:
        window(std::string_view title, int width, int height);
        ~window();

        SDL_Window* get_window() const;

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

    inline SDL_Window* window::get_window() const {
        return m_window;
    }

    inline bool window::is_running() const {
        return m_is_running;
    }

    inline void window::set_is_running(bool is_running) {
        m_is_running = is_running;
    }

}  // namespace engine