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

    void renderer::sprite_draw(const sprite_texture& sprite, const glm::vec2& position) {
        if (sprite.is_valid() == false) {
            return;
        }

        SDL_FRect dst_rect = {position.x, position.y, sprite.get_size().x, sprite.get_size().y};

        SDL_RenderTexture(m_sdl_renderer, sprite.get_sdl_texture(), nullptr, &dst_rect);
    }

    glm::vec2 renderer::get_output_size() const {
        int width, height;
        SDL_GetCurrentRenderOutputSize(m_sdl_renderer, &width, &height);
        return {static_cast<float>(width), static_cast<float>(height)};
    }
}  // namespace engine
