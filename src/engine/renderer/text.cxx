#include "text.hxx"

#include <SDL3_ttf/SDL_ttf.h>

namespace engine {
    game_text_static::game_text_static(TTF_Text* sdl_text)
        : m_sdl_text(sdl_text), m_origin(0.0f, 0.0f) {
        if (m_sdl_text == nullptr) {
            throw std::invalid_argument("Invalid SDL text object");
        }
    }

    game_text_static::~game_text_static() {
        if (m_sdl_text != nullptr) {
            TTF_DestroyText(m_sdl_text);
        }
    }

    game_text_static::game_text_static(game_text_static&& other) noexcept
        : m_sdl_text(other.m_sdl_text), m_origin(other.m_origin) {
        other.m_sdl_text = nullptr;
        other.m_origin = glm::vec2(0.0f);
    }

    game_text_static& game_text_static::operator=(game_text_static&& other) noexcept {
        if (this != &other) {
            if (m_sdl_text != nullptr) {
                TTF_DestroyText(m_sdl_text);
            }
            m_sdl_text = other.m_sdl_text;
            m_origin = other.m_origin;
            other.m_sdl_text = nullptr;
            other.m_origin = glm::vec2(0.0f);
        }
        return *this;
    }

    game_color game_text_static::get_color() const {
        game_color color = {};
        TTF_GetTextColor(m_sdl_text, &color.r, &color.g, &color.b, &color.a);
        return color;
    }

    glm::vec2 game_text_static::get_size() const {
        int w, h;
        TTF_GetTextSize(m_sdl_text, &w, &h);
        return {static_cast<float>(w), static_cast<float>(h)};
    }

    void game_text_static::set_text_raw(std::string_view new_text) {
        TTF_SetTextString(m_sdl_text, new_text.data(), new_text.length());
    }

    void game_text_static::set_color(const game_color& new_color) {
        TTF_SetTextColor(m_sdl_text, new_color.r, new_color.g, new_color.b, new_color.a);
    }

    game_text_dynamic::game_text_dynamic(std::string_view content,
                                         std::unique_ptr<game_text_static> static_text,
                                         SDL_Renderer* sdl_renderer, TTF_Font* font)
        : m_static_text(std::move(static_text)),
          m_sdl_renderer(sdl_renderer),
          m_sdl_font(font),
          m_cached_texture(nullptr),
          m_is_texture_dirty(true),
          m_text_content(content),
          m_scale(1.0f, 1.0f),
          m_rotation_degrees(0.0f) {
        if (m_static_text == nullptr) {
            throw std::invalid_argument("Invalid static text object");
        }

        if (m_text_content.empty() == true) {
            throw std::invalid_argument("Invalid text content");
        }

        if (m_sdl_renderer == nullptr) {
            throw std::invalid_argument("Invalid renderer");
        }

        if (m_sdl_font == nullptr) {
            throw std::invalid_argument("Invalid font");
        }
    }

    game_text_dynamic::~game_text_dynamic() {
        if (m_cached_texture != nullptr) {
            SDL_DestroyTexture(m_cached_texture);
        }
    }

    SDL_Texture* game_text_dynamic::get_sdl_texture() const {
        // Ugly but we need to cast away const to call regenerate_texture_if_needed.
        const_cast<game_text_dynamic*>(this)->regenerate_texture_if_needed();
        return m_cached_texture;
    }

    void game_text_dynamic::set_text_raw(std::string_view new_text) {
        m_text_content = new_text;
        m_static_text->set_text_raw(new_text);
        mark_texture_dirty();
    }

    void game_text_dynamic::set_color(const game_color& new_color) {
        m_static_text->set_color(new_color);
        mark_texture_dirty();
    }

    void game_text_dynamic::regenerate_texture_if_needed() {
        // Text hasn't changed. No need to regenerate texture.
        if (m_is_texture_dirty == false) {
            return;
        }

        // Clean up the old texture.
        if (m_cached_texture != nullptr) {
            SDL_DestroyTexture(m_cached_texture);
            m_cached_texture = nullptr;
        }

        m_cached_texture = create_texture_from_surface();
        m_is_texture_dirty = false;
    }

    SDL_Texture* game_text_dynamic::create_texture_from_surface() {
        SDL_Surface* surface =
            TTF_RenderText_Blended(m_sdl_font, m_text_content.c_str(), m_text_content.size(),
                                   m_static_text->get_color().to_sdl_color());
        if (surface == nullptr) {
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_sdl_renderer, surface);

        if (texture != nullptr) {
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        }

        SDL_DestroySurface(surface);

        return texture;
    }
}  // namespace engine
