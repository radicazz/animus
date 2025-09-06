#include "renderer.hxx"

#include "../camera/camera.hxx"

#include <SDL3_ttf/SDL_ttf.h>

#include <stdexcept>

namespace engine {
    game_renderer::game_renderer(SDL_Window* window) : m_sdl_renderer(nullptr), m_camera(nullptr) {
        if (m_sdl_renderer = SDL_CreateRenderer(window, nullptr); m_sdl_renderer == nullptr) {
            throw std::runtime_error("Failed to create renderer");
        }

        if (TTF_Init() == false) {
            throw std::runtime_error("Failed to initialize SDL_ttf.");
        }

        m_sdl_text_engine = TTF_CreateRendererTextEngine(m_sdl_renderer);
        if (m_sdl_text_engine == nullptr) {
            throw std::runtime_error("Failed to create TTF text engine.");
        }
    }

    game_renderer::~game_renderer() {
        SDL_DestroyRenderer(m_sdl_renderer);
        TTF_DestroyRendererTextEngine(m_sdl_text_engine);
        TTF_Quit();
    }

    void game_renderer::frame_begin() {
        SDL_SetRenderDrawColor(m_sdl_renderer, 0, 0, 0, 255);
        SDL_RenderClear(m_sdl_renderer);
    }

    void game_renderer::frame_end() {
        SDL_RenderPresent(m_sdl_renderer);
    }

    void game_renderer::sprite_draw_world(const render_sprite* sprite,
                                          const glm::vec2& world_position) {
        if (sprite == nullptr || sprite->is_valid() == false) {
            return;
        }

        glm::vec2 screen_position;

        if (m_camera != nullptr) {
            // Transform world position to screen coordinates using camera
            screen_position = m_camera->world_to_screen(world_position);

            // Frustum culling: skip drawing if sprite is outside view
            if (m_camera->is_in_view(world_position, sprite->get_size()) == false) {
                return;
            }
        } else {
            // No camera set, use world position as screen position
            screen_position = world_position;
        }

        sprite_draw_screen(sprite, screen_position);
    }

    void game_renderer::sprite_draw_screen(const render_sprite* sprite,
                                           const glm::vec2& screen_position) {
        if (sprite == nullptr || sprite->is_valid() == false) {
            return;
        }

        const glm::vec2 size = sprite->get_size();
        const glm::vec2 origin = sprite->get_origin();

        // Apply camera zoom to sprite size and origin to make sprites scale with zoom
        glm::vec2 final_size = size;
        glm::vec2 final_origin = origin;

        if (m_camera != nullptr) {
            float zoom = m_camera->get_zoom();
            final_size *= zoom;
            final_origin *= zoom;
        }

        const SDL_FRect dst_rect = {screen_position.x - final_origin.x,
                                    screen_position.y - final_origin.y, final_size.x, final_size.y};
        const SDL_FPoint center = {final_origin.x, final_origin.y};

        // TODO: Add options for flipping with SDL_FLIP_NONE.

        SDL_RenderTextureRotated(m_sdl_renderer, sprite->get_sdl_texture(), nullptr, &dst_rect,
                                 sprite->get_rotation(), &center, SDL_FLIP_NONE);
    }

    void game_renderer::sprite_draw_raw(const render_sprite* sprite,
                                        const glm::vec2& screen_position) {
        if (sprite == nullptr || sprite->is_valid() == false) {
            return;
        }

        const glm::vec2 size = sprite->get_size();
        const glm::vec2 origin = sprite->get_origin();

        // No camera transformations at all - raw screen coordinates
        const SDL_FRect dst_rect = {screen_position.x - origin.x, screen_position.y - origin.y,
                                    size.x, size.y};
        const SDL_FPoint center = {origin.x, origin.y};

        SDL_RenderTextureRotated(m_sdl_renderer, sprite->get_sdl_texture(), nullptr, &dst_rect,
                                 sprite->get_rotation(), &center, SDL_FLIP_NONE);
    }

    void game_renderer::text_draw_world(const render_text* text, const glm::vec2& world_position) {
        if (text == nullptr || text->is_valid() == false) {
            return;
        }

        glm::vec2 screen_position;

        if (m_camera != nullptr) {
            // Transform world position to screen coordinates using camera
            screen_position = m_camera->world_to_screen(world_position);

            // Frustum culling: skip drawing if text is outside view
            // Account for text scale and origin when calculating bounds
            const glm::vec2 text_size = text->get_size();
            // const glm::vec2 text_scale = text->get_scale();
            const glm::vec2 scaled_size = text_size * 1.f /*text_scale*/;

            if (m_camera->is_in_view(world_position, scaled_size) == false) {
                return;
            }
        } else {
            screen_position = world_position;
        }

        text_draw_screen(text, screen_position);
    }

    void game_renderer::text_draw_screen(const render_text* text,
                                         const glm::vec2& screen_position) {
        // TODO: These checks run twice as text_draw_world also calls this function. Figure out a
        // fix.
        if (text == nullptr || text->is_valid() == false) {
            return;
        }

        const glm::vec2 text_size = text->get_size();
        const glm::vec2 text_origin = text->get_origin();

        // TODO: Figure out scaling with SDL3 TTF_Text.
        // glm::vec2 text_scale = text->get_scale();

        // Apply camera zoom to text scale
        // if (m_camera != nullptr) {
        // float zoom = m_camera->get_zoom();
        // text_scale *= zoom;
        // }

        const glm::vec2 final_size = text_size * 1.0f /*scale*/;
        const glm::vec2 final_origin = text_origin * 1.0f /*scale*/;

        // Calculate final position accounting for origin
        const glm::vec2 final_position = screen_position - final_origin;

        TTF_Text* sdl_text = text->get_sdl_text();

        TTF_DrawRendererText(sdl_text, final_position.x, final_position.y);
    }
}  // namespace engine
