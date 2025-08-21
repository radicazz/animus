#include "window.hxx"

#include <stdexcept>

namespace engine {
    window::window(std::string_view title, int width, int height)
        : m_window(nullptr), m_is_running(true) {
        if (SDL_Init(SDL_INIT_VIDEO) == false) {
            throw std::runtime_error("Failed to initialize SDL.");
        }

        if (m_window = SDL_CreateWindow(title.data(), width, height, 0); m_window == nullptr) {
            throw std::runtime_error("Failed to create window.");
        }

        if (TTF_Init() == false) {
            throw std::runtime_error("Failed to initialize SDL_ttf.");
        }
    }

    window::~window() {
        TTF_Quit();
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

    std::string window::get_title() const {
        return SDL_GetWindowTitle(m_window);
    }

    void window::set_title(std::string_view new_title) {
        SDL_SetWindowTitle(m_window, new_title.data());
    }

    glm::vec2 window::get_size() const {
        int width, height;
        SDL_GetWindowSize(m_window, &width, &height);
        return {static_cast<float>(width), static_cast<float>(height)};
    }
}  // namespace engine
