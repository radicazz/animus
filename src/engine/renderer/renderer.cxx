#include "renderer.hxx"
#include "../camera/camera.hxx"

#include <stdexcept>

namespace engine {
    renderer::renderer(SDL_Window* window) : m_sdl_renderer(nullptr), m_camera(nullptr) {
        if (m_sdl_renderer = SDL_CreateRenderer(window, nullptr); m_sdl_renderer == nullptr) {
            throw std::runtime_error("Failed to create renderer");
        }
    }

    renderer::~renderer() {
        SDL_DestroyRenderer(m_sdl_renderer);
    }

    void renderer::frame_begin() {
        SDL_SetRenderDrawColor(m_sdl_renderer, 0, 0, 0, 255);
        SDL_RenderClear(m_sdl_renderer);
    }

    void renderer::frame_end() {
        SDL_RenderPresent(m_sdl_renderer);
    }

    void renderer::set_camera(const camera* cam) {
        m_camera = cam;
    }

    const camera* renderer::get_camera() const {
        return m_camera;
    }

    void renderer::sprite_draw_world(const game_sprite* sprite, const glm::vec2& world_position) {
        if (sprite == nullptr || sprite->is_valid() == false) {
            return;
        }

        glm::vec2 screen_position;
        
        if (m_camera != nullptr) {
            // Transform world position to screen coordinates using camera
            screen_position = m_camera->world_to_screen(world_position);
            
            // Frustum culling: skip drawing if sprite is outside view
            if (!m_camera->is_in_view(world_position, sprite->get_size())) {
                return;
            }
        } else {
            // No camera set, use world position as screen position
            screen_position = world_position;
        }

        sprite_draw_screen(sprite, screen_position);
    }

    void renderer::sprite_draw_screen(const game_sprite* sprite, const glm::vec2& screen_position) {
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

        const SDL_FRect dst_rect = {
            screen_position.x - final_origin.x, 
            screen_position.y - final_origin.y, 
            final_size.x, 
            final_size.y
        };
        const SDL_FPoint center = {final_origin.x, final_origin.y};

        // TODO: Add options for flipping with SDL_FLIP_NONE.

        SDL_RenderTextureRotated(m_sdl_renderer, sprite->get_sdl_texture(), nullptr, &dst_rect,
                                 sprite->get_rotation(), &center, SDL_FLIP_NONE);
    }

    void renderer::sprite_draw_raw(const game_sprite* sprite, const glm::vec2& screen_position) {
        if (sprite == nullptr || sprite->is_valid() == false) {
            return;
        }

        const glm::vec2 size = sprite->get_size();
        const glm::vec2 origin = sprite->get_origin();

        // No camera transformations at all - raw screen coordinates
        const SDL_FRect dst_rect = {
            screen_position.x - origin.x, 
            screen_position.y - origin.y, 
            size.x, 
            size.y
        };
        const SDL_FPoint center = {origin.x, origin.y};

        SDL_RenderTextureRotated(m_sdl_renderer, sprite->get_sdl_texture(), nullptr, &dst_rect,
                                 sprite->get_rotation(), &center, SDL_FLIP_NONE);
    }

    glm::vec2 renderer::get_output_size() const {
        int width, height;
        SDL_GetCurrentRenderOutputSize(m_sdl_renderer, &width, &height);
        return {static_cast<float>(width), static_cast<float>(height)};
    }
}  // namespace engine
