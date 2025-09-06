#include "text.hxx"

#include <SDL3_ttf/SDL_ttf.h>

namespace engine {
    render_text::render_text(TTF_Text* sdl_text) : m_sdl_text(sdl_text), m_origin(0.0f, 0.0f) {
    }

    render_text::~render_text() {
        if (m_sdl_text) {
            TTF_DestroyText(m_sdl_text);
        }
    }

    render_color render_text::get_color() const {
        render_color color = {};
        TTF_GetTextColor(m_sdl_text, &color.r, &color.g, &color.b, &color.a);
        return color;
    }

    [[nodiscard]] glm::vec2 render_text::get_size() const {
        int w, h;
        TTF_GetTextSize(m_sdl_text, &w, &h);
        return {static_cast<float>(w), static_cast<float>(h)};
    }

    void render_text::set_text_raw(std::string_view new_text) {
        TTF_SetTextString(m_sdl_text, new_text.data(), new_text.length());
    }

    void render_text::set_color(const render_color& new_color) {
        TTF_SetTextColor(m_sdl_text, new_color.r, new_color.g, new_color.b, new_color.a);
    }
}  // namespace engine