#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <string>

namespace engine {
    class game_sprite {
    public:
        game_sprite() = delete;
        game_sprite(std::string_view file_path, SDL_Renderer* renderer);
        game_sprite(std::string_view file_path, SDL_Renderer* renderer, const glm::vec2& size);
        game_sprite(std::string_view file_path, SDL_Renderer* renderer, const glm::vec2& size,
                    const glm::vec2& origin, float rotation);
        ~game_sprite();

        [[nodiscard]] SDL_Texture* get_sdl_texture() const;
        void set_sdl_texture(SDL_Texture* texture);

        [[nodiscard]] std::string_view get_file_path() const;
        [[nodiscard]] glm::vec2 get_size() const;
        [[nodiscard]] glm::vec2 get_origin() const;
        [[nodiscard]] float get_rotation() const;

        void set_size(const glm::vec2& size);
        void set_origin(const glm::vec2& origin);
        void set_rotation(float rotation);

        [[nodiscard]] bool is_valid() const;

        void unload();

    private:
        void auto_size_and_origin();

    private:
        std::string m_file_path;     // File path of the sprite image.
        SDL_Texture* m_sdl_texture;  // Underlying SDL_Texture.
        glm::vec2 m_size;            // Size of the image that makes the sprite.
        glm::vec2 m_origin;          // Origin point of the sprite (automatically centered).
        float m_rotation;            // Rotation angle of the sprite.
    };

    inline SDL_Texture* game_sprite::get_sdl_texture() const {
        return m_sdl_texture;
    }

    inline void game_sprite::set_sdl_texture(SDL_Texture* texture) {
        m_sdl_texture = texture;
    }

    inline std::string_view game_sprite::get_file_path() const {
        return m_file_path;
    }

    inline glm::vec2 game_sprite::get_size() const {
        return m_size;
    }

    inline glm::vec2 game_sprite::get_origin() const {
        return m_origin;
    }

    inline float game_sprite::get_rotation() const {
        return m_rotation;
    }

    inline void game_sprite::set_size(const glm::vec2& size) {
        m_size = size;
    }

    inline void game_sprite::set_origin(const glm::vec2& origin) {
        m_origin = origin;
    }

    inline void game_sprite::set_rotation(float rotation) {
        m_rotation = rotation;
    }

    inline bool game_sprite::is_valid() const {
        return m_sdl_texture != nullptr;
    }

    class sprite_texture_animated {};
}  // namespace engine
