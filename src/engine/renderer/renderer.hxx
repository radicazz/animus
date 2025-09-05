#pragma once

#include "sprite.hxx"

namespace engine {
    class camera; // Forward declaration

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

        // Set the camera for rendering transformations
        void set_camera(const camera* cam);
        [[nodiscard]] const camera* get_camera() const;

        // Draw sprite with world coordinates (camera will transform)
        void sprite_draw_world(const game_sprite* sprite, const glm::vec2& world_position);
        
        // Draw sprite directly in screen coordinates (ignores camera)
        void sprite_draw_screen(const game_sprite* sprite, const glm::vec2& screen_position);
        
        // Draw sprite without any camera transformations (raw screen coordinates)
        void sprite_draw_raw(const game_sprite* sprite, const glm::vec2& screen_position);

        /**
         * @brief Get the output size of the renderer.
         *
         * @return `glm::vec2` representing the output size in pixels.
         */
        [[nodiscard]] glm::vec2 get_output_size() const;

    private:
        SDL_Renderer* m_sdl_renderer;
        const camera* m_camera; // Camera reference for transformations
    };

    inline SDL_Renderer* renderer::get_sdl_renderer() const {
        return m_sdl_renderer;
    }

}  // namespace engine
