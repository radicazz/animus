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

        // Apply camera zoom to sprite size and origin to make sprites scale with zoom
        glm::vec2 final_size = sprite->get_size();
        glm::vec2 final_origin = sprite->get_origin();

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

    void game_renderer::sprite_draw_screen(const render_sprite* sprite,
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

    void game_renderer::text_draw_world(const render_text_dynamic* text,
                                        const glm::vec2& world_position) {
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
            const glm::vec2 text_scale = text->get_scale();
            const float camera_zoom = m_camera->get_zoom();
            const glm::vec2 total_scale = text_scale * camera_zoom;
            const glm::vec2 scaled_size = text_size * total_scale;

            if (m_camera->is_in_view(world_position, scaled_size) == false) {
                return;
            }
        } else {
            screen_position = world_position;
        }

        // Delegate to screen drawing with camera zoom applied
        text_draw_screen(text, screen_position);
    }

    void game_renderer::text_draw_screen(const render_text_dynamic* text,
                                         const glm::vec2& screen_position) {
        if (text == nullptr || text->is_valid() == false) {
            return;
        }

        SDL_Texture* texture = text->get_sdl_texture();
        if (!texture) {
            return;
        }

        // Get text properties
        const glm::vec2 text_size = text->get_size();
        const glm::vec2 text_origin = text->get_origin();
        const glm::vec2 text_scale = text->get_scale();
        const float text_rotation = text->get_rotation();

        // Apply camera zoom to scale if camera is present
        glm::vec2 final_scale = text_scale;
        if (m_camera != nullptr) {
            final_scale *= m_camera->get_zoom();
        }

        // Calculate final size and position
        const glm::vec2 final_size = text_size * final_scale;
        const glm::vec2 scaled_origin = text_origin * final_scale;
        const glm::vec2 final_position = screen_position - scaled_origin;

        // Set up destination rectangle
        SDL_FRect dest_rect = {final_position.x, final_position.y, final_size.x, final_size.y};

        // Set up rotation center point (relative to destination rectangle)
        SDL_FPoint center = {scaled_origin.x, scaled_origin.y};

        // Render with transformations
        if (text_rotation != 0.0f) {
            // Render with rotation
            SDL_RenderTextureRotated(m_sdl_renderer, texture, nullptr, &dest_rect, text_rotation,
                                     &center, SDL_FLIP_NONE);
        } else {
            // Simple render without rotation (slightly more efficient)
            SDL_RenderTexture(m_sdl_renderer, texture, nullptr, &dest_rect);
        }
    }

    void game_renderer::text_draw_screen(const render_text_static* text,
                                         const glm::vec2& screen_position) {
        if (text == nullptr || text->is_valid() == false) {
            return;
        }

        const glm::vec2 text_origin = text->get_origin();

        TTF_DrawRendererText(text->get_sdl_text(), screen_position.x - text_origin.x,
                             screen_position.y - text_origin.y);
    }
}  // namespace engine
