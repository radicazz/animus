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

    void renderer::begin_frame() {
        SDL_SetRenderDrawColor(m_sdl_renderer, 0, 0, 0, 255);
        SDL_RenderClear(m_sdl_renderer);
    }

    void renderer::end_frame() {
        SDL_RenderPresent(m_sdl_renderer);
    }
}  // namespace engine
