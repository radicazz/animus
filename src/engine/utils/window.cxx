#include "window.hxx"

#include <stdexcept>

namespace engine {
    game_window::game_window(std::string_view title, const glm::ivec2& size) : m_window(nullptr) {
        if (SDL_Init(SDL_INIT_VIDEO) == false) {
            throw std::runtime_error("Failed to initialize SDL.");
        }

        // TODO: Add window flags to public API?
        constexpr SDL_WindowFlags window_flags = {};

        if (m_window = SDL_CreateWindow(title.data(), size.x, size.y, window_flags);
            m_window == nullptr) {
            throw std::runtime_error("Failed to create window.");
        }
    }

    game_window::~game_window() {
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

    std::string game_window::get_title() const {
        return SDL_GetWindowTitle(m_window);
    }

    void game_window::set_title(std::string_view new_title) {
        SDL_SetWindowTitle(m_window, new_title.data());
    }

    glm::vec2 game_window::get_logical_size() const {
        int width, height;
        SDL_GetWindowSize(m_window, &width, &height);
        return {static_cast<float>(width), static_cast<float>(height)};
    }

    glm::vec2 game_window::get_pixel_size() const {
        int width, height;
        SDL_GetWindowSizeInPixels(m_window, &width, &height);
        return {static_cast<float>(width), static_cast<float>(height)};
    }
}  // namespace engine
