#include "font.hxx"

#include <stdexcept>
#include <format>

namespace engine {
    game_font::game_font(std::string_view file_path, float size) {
        m_font = TTF_OpenFont(file_path.data(), size);
        if (m_font == nullptr) {
            throw std::runtime_error(std::format("Failed to load font: {}", file_path));
        }
    }

    game_font::~game_font() {
        if (m_font != nullptr) {
            TTF_CloseFont(m_font);
        }
    }
}