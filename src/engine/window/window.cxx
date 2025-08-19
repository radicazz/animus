#include "window.hxx"

#include <stdexcept>

namespace engine {
    renderer::renderer(SDL_Window* window) : m_renderer(nullptr) {
        // Change the name to select a specific renderer.
        if (m_renderer = SDL_CreateRenderer(window, nullptr); m_renderer == nullptr) {
            throw std::runtime_error("Failed to create renderer");
        }
    }

    renderer::~renderer() {
        SDL_DestroyRenderer(m_renderer);
    }

    void renderer::render_pre() {
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
        SDL_RenderClear(m_renderer);
    }

    void renderer::render_post() {
        SDL_RenderPresent(m_renderer);
    }

    void renderer::set_draw_color(const renderer_color& color) {
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    }

    void renderer::draw_rect(const glm::vec2& position, const glm::vec2& size) {
        const SDL_FRect rect = {.x = position.x, .y = position.y, .w = size.x, .h = size.y};
        SDL_assert(SDL_RenderFillRect(m_renderer, &rect));
    }

    void renderer::draw_rect(const glm::vec2& position, const glm::vec2& size,
                             const renderer_color& color) {
        set_draw_color(color);
        draw_rect(position, size);
    }

    window::window(std::string_view title, int width, int height)
        : m_window(nullptr), m_is_running(true) {
        if (SDL_Init(SDL_INIT_VIDEO) == false) {
            throw std::runtime_error("Failed to initialize SDL.");
        }

        if (m_window = SDL_CreateWindow(title.data(), width, height, 0); m_window == nullptr) {
            throw std::runtime_error("Failed to create window.");
        }
    }

    window::~window() {
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }
}  // namespace engine
