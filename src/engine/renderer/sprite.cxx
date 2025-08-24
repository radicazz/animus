#include "sprite.hxx"

#include <SDL3_image/SDL_image.h>

#include <stdexcept>
#include <format>

namespace engine {
    game_sprite::game_sprite(std::string_view file_path, SDL_Renderer* renderer)
        : m_sdl_texture(nullptr),
          m_file_path(file_path),
          m_size{0, 0},
          m_origin{0, 0},
          m_rotation(0.f) {
        // TODO: Make this an internal function to be used in other ctors.
        SDL_Texture* texture = IMG_LoadTexture(renderer, file_path.data());
        if (texture == nullptr) {
            throw std::runtime_error(std::format("Failed to load the texture at: {}", file_path));
        }

        m_sdl_texture = texture;

        auto_size_and_origin();
    }

    game_sprite::game_sprite(std::string_view file_path, SDL_Renderer* renderer,
                             const glm::vec2& size)
        : m_sdl_texture(nullptr),
          m_file_path(file_path),
          m_size(size),
          m_origin{0, 0},
          m_rotation(0.f) {
        SDL_Texture* texture = IMG_LoadTexture(renderer, file_path.data());
        if (texture == nullptr) {
            throw std::runtime_error(std::format("Failed to load the texture at: {}", file_path));
        }

        m_sdl_texture = texture;

        auto_size_and_origin();
    }

    game_sprite::game_sprite(std::string_view file_path, SDL_Renderer* renderer,
                             const glm::vec2& size, const glm::vec2& origin, float rotation)
        : m_sdl_texture(nullptr),
          m_file_path(file_path),
          m_size(size),
          m_origin(origin),
          m_rotation(rotation) {
        SDL_Texture* texture = IMG_LoadTexture(renderer, file_path.data());
        if (texture == nullptr) {
            throw std::runtime_error(std::format("Failed to load the texture at: {}", file_path));
        }

        m_sdl_texture = texture;

        auto_size_and_origin();
    }

    game_sprite::~game_sprite() {
        unload();
    }

    void game_sprite::unload() {
        if (m_sdl_texture != nullptr) {
            SDL_DestroyTexture(m_sdl_texture);
            m_sdl_texture = nullptr;
        }
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
