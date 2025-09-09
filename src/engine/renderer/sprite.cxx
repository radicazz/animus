#include "sprite.hxx"

#include <SDL3_image/SDL_image.h>

#include <stdexcept>
#include <format>

namespace engine {
    game_sprite::game_sprite(std::string_view file_path, SDL_Texture* texture)
        : m_file_path(file_path),
          m_sdl_texture(texture),
          m_size{0, 0},
          m_origin{0, 0},
          m_scale{1.0f, 1.0f},
          m_rotation(0.f) {
        auto_size_and_origin();
    }

    game_sprite::game_sprite(std::string_view file_path, SDL_Texture* texture,
                             const glm::vec2& size)
        : m_file_path(file_path),
          m_sdl_texture(texture),
          m_size(size),
          m_origin(size * 0.5f),
          m_scale{1.0f, 1.0f},
          m_rotation(0.f) {
    }

    void game_sprite::auto_size_and_origin() {
        if (is_valid() == false) {
            return;
        }

        float w, h;
        SDL_GetTextureSize(m_sdl_texture, &w, &h);

        m_size = {w, h};
        m_origin = m_size * 0.5f;
    }
}  // namespace engine
