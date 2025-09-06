#pragma once

#include <glm/glm.hpp>
#include <string>

struct SDL_Texture;

namespace engine {
    /**
     * @brief Represents a 2D sprite in the game.
     *
     * This class encapsulates the properties and behaviors of a 2D sprite,
     * including its texture, size, origin, and rotation.
     *
     * Use the `resource_manager` to create a sprite, passing in the file path of your texture. The
     * underlying texture will be loaded and managed by the resource manager automatically so there
     * is no need to worry about memory management.
     */
    class render_sprite {
    public:
        render_sprite() = delete;
        render_sprite(std::string_view file_path, SDL_Texture* texture);
        render_sprite(std::string_view file_path, SDL_Texture* texture, const glm::vec2& size);

        [[nodiscard]] SDL_Texture* get_sdl_texture() const;
        [[nodiscard]] std::string_view get_file_path() const;

        [[nodiscard]] glm::vec2 get_size() const;
        [[nodiscard]] glm::vec2 get_origin() const;
        [[nodiscard]] float get_rotation() const;

        void set_size(const glm::vec2& size);
        void set_origin(const glm::vec2& origin);
        void set_rotation(float rotation);

        [[nodiscard]] bool is_valid() const;

    private:
        void auto_size_and_origin();

    private:
        std::string m_file_path;     // File path of the sprite image.
        SDL_Texture* m_sdl_texture;  // Underlying SDL_Texture.
        glm::vec2 m_size;            // Size of the image that makes the sprite.
        glm::vec2 m_origin;          // Origin point of the sprite (automatically centered).
        float m_rotation;            // Rotation angle of the sprite.
    };

    inline SDL_Texture* render_sprite::get_sdl_texture() const {
        return m_sdl_texture;
    }

    inline std::string_view render_sprite::get_file_path() const {
        return m_file_path;
    }

    inline glm::vec2 render_sprite::get_size() const {
        return m_size;
    }

    inline glm::vec2 render_sprite::get_origin() const {
        return m_origin;
    }

    inline float render_sprite::get_rotation() const {
        return m_rotation;
    }

    inline void render_sprite::set_size(const glm::vec2& size) {
        m_size = size;
    }

    inline void render_sprite::set_origin(const glm::vec2& origin) {
        m_origin = origin;
    }

    inline void render_sprite::set_rotation(float rotation) {
        m_rotation = rotation;
    }

    inline bool render_sprite::is_valid() const {
        return m_sdl_texture != nullptr;
    }
}  // namespace engine
