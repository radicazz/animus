#pragma once

#include "sprite.hxx"
#include "text.hxx"

struct TTF_TextEngine;

namespace engine {
    class game_camera;
    class game_viewport;

    /**
     * @brief Handles rendering of sprites and text with support for camera and viewport.
     */
    class game_renderer {
    public:
        explicit game_renderer(SDL_Window* window);
        ~game_renderer();

        // Resource management - disable copy, enable move
        game_renderer(const game_renderer&) = delete;
        game_renderer& operator=(const game_renderer&) = delete;
        game_renderer(game_renderer&& other) noexcept;
        game_renderer& operator=(game_renderer&& other) noexcept;

        [[nodiscard]] SDL_Renderer* get_sdl_renderer() const;
        [[nodiscard]] TTF_TextEngine* get_sdl_text_engine() const;

        void frame_begin();
        void frame_end();

        void set_camera(const game_camera* camera);
        [[nodiscard]] const game_camera* get_camera() const;

        void set_viewport(const game_viewport* viewport);
        [[nodiscard]] const game_viewport* get_viewport() const;

        void sprite_draw_world(const game_sprite::uptr& sprite, const glm::vec2& world_position);
        void sprite_draw_screen(const game_sprite::uptr& sprite, const glm::vec2& screen_position);

        void text_draw_world(const game_text_dynamic::uptr& text, const glm::vec2& world_position);
        void text_draw_screen(const game_text_dynamic::uptr& text,
                              const glm::vec2& screen_position);

        void text_draw_screen(const game_text_static::uptr& text, const glm::vec2& screen_position);

        /**
         * @brief Get the output size of the renderer.
         * @return glm::vec2 representing the output size in pixels.
         */
        [[nodiscard]] glm::vec2 get_output_size() const;

    private:
        SDL_Renderer* m_sdl_renderer;
        TTF_TextEngine* m_sdl_text_engine;
        const game_camera* m_camera;
        const game_viewport* m_viewport;
    };

    inline SDL_Renderer* game_renderer::get_sdl_renderer() const {
        return m_sdl_renderer;
    }
    inline TTF_TextEngine* game_renderer::get_sdl_text_engine() const {
        return m_sdl_text_engine;
    }

    inline void game_renderer::set_camera(const game_camera* cam) {
        m_camera = cam;
    }
    inline const game_camera* game_renderer::get_camera() const {
        return m_camera;
    }

    inline void game_renderer::set_viewport(const game_viewport* vp) {
        m_viewport = vp;
    }
    inline const game_viewport* game_renderer::get_viewport() const {
        return m_viewport;
    }

}  // namespace engine
