#include "sprite.hxx"

namespace engine {
    sprite_texture::sprite_texture()
        : m_sdl_texture(nullptr),
          m_source_rect{0, 0, 0, 0},
          m_size{0, 0},
          m_origin{0, 0},
          m_rotation(0) {
    }

    sprite_texture::sprite_texture(SDL_Texture* texture)
        : m_sdl_texture(texture),
          m_source_rect{0, 0, 0, 0},
          m_size{0, 0},
          m_origin{0, 0},
          m_rotation(0) {
        if (is_valid() == true) {
            auto_size();
        }
    }

    sprite_texture::sprite_texture(SDL_Texture* texture, const glm::vec2& size)
        : m_sdl_texture(texture),
          m_source_rect{0, 0, 0, 0},
          m_size(size),
          m_origin{0, 0},
          m_rotation(0) {
    }

    void sprite_texture::auto_size() {
        if (is_valid() == false) {
            return;
        }

        float w, h;
        SDL_GetTextureSize(m_sdl_texture, &w, &h);
        m_size = {w, h};
    }
}  // namespace engine
