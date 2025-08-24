#include "renderer.hxx"

#include <stdexcept>

namespace engine {
    renderer::renderer(SDL_Window* window) : m_sdl_renderer(nullptr) {
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

    void renderer::sprite_draw(const game_sprite* sprite, const glm::vec2& position) {
        if (sprite == nullptr || sprite->is_valid() == false) {
            return;
        }

        const glm::vec2 size = sprite->get_size();
        const glm::vec2 origin = sprite->get_origin();

        const SDL_FRect dst_rect = {position.x - origin.x, position.y - origin.y, size.x, size.y};
        const SDL_FPoint center = {origin.x, origin.y};

        // TODO: Add options for flipping with SDL_FLIP_NONE.

        SDL_RenderTextureRotated(m_sdl_renderer, sprite->get_sdl_texture(), nullptr, &dst_rect,
                                 sprite->get_rotation(), &center, SDL_FLIP_NONE);
    }

    glm::vec2 renderer::get_output_size() const {
        int width, height;
        SDL_GetCurrentRenderOutputSize(m_sdl_renderer, &width, &height);
        return {static_cast<float>(width), static_cast<float>(height)};
    }
}  // namespace engine
