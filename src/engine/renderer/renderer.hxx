#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace engine {
    class renderer {
    public:
        renderer(SDL_Window* window);
        ~renderer();

        /**
         * @brief Access the internal SDL_Renderer.
         *
         * @return `SDL_Renderer*` Pointer to the SDL_Renderer.
         *
         * @note Should only be used internally for the engine.
         *
         */
        [[nodiscard]] SDL_Renderer* get_sdl_renderer() const;

        void frame_begin();
        void frame_end();

        /**
         * @brief Get the output size of the renderer.
         *
         * @return `glm::vec2` representing the output size in pixels.
         */
        [[nodiscard]] glm::vec2 get_output_size() const;

    private:
        SDL_Renderer* m_sdl_renderer;
    };

    inline SDL_Renderer* renderer::get_sdl_renderer() const {
        return m_sdl_renderer;
    }

}  // namespace engine
