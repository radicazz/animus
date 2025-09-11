#include "window.hxx"
#include "../logger.hxx"

#include <stdexcept>

namespace engine {
    game_window::game_window(std::string_view title, const glm::ivec2& size) : m_window(nullptr) {
        if (SDL_Init(SDL_INIT_VIDEO) == false) {
            throw std::runtime_error("Failed to initialize SDL.");
        }

        // Initialize SDL's log priorities to match our compile-time settings
        initialize_logging();

        game_log("SDL initialized successfully.");

        // TODO: Add window flags to public API?
        constexpr SDL_WindowFlags window_flags = {};

        if (m_window = SDL_CreateWindow(title.data(), size.x, size.y, window_flags);
            m_window == nullptr) {
            throw std::runtime_error("Failed to create window.");
        }

        game_log("Window created: '{}' ({}x{})", title, size.x, size.y);
    }

    game_window::~game_window() {
        if (m_window) {
            SDL_DestroyWindow(m_window);
            game_log("Window destroyed.");
        }

        SDL_Quit();
        game_log("SDL quit.");
    }

    game_window::game_window(game_window&& other) noexcept : m_window(other.m_window) {
        other.m_window = nullptr;
    }

    game_window& game_window::operator=(game_window&& other) noexcept {
        if (this != &other) {
            if (m_window) {
                SDL_DestroyWindow(m_window);
            }
            m_window = other.m_window;
            other.m_window = nullptr;
        }

        return *this;
    }

    std::string game_window::get_title() const {
        return SDL_GetWindowTitle(m_window);
    }

    void game_window::set_title(std::string_view new_title) {
        SDL_SetWindowTitle(m_window, new_title.data());
    }

    glm::ivec2 game_window::get_logical_size() const {
        glm::ivec2 size;
        SDL_GetWindowSize(m_window, &size.x, &size.y);
        return size;
    }

    void game_window::set_logical_size(const glm::ivec2& size) {
        SDL_SetWindowSize(m_window, size.x, size.y);
    }

    glm::ivec2 game_window::get_pixel_size() const {
        glm::ivec2 size;
        SDL_GetWindowSizeInPixels(m_window, &size.x, &size.y);
        return size;
    }
}  // namespace engine
