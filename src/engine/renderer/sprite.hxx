#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <string>

namespace engine {
    class sprite_texture {
    public:
        sprite_texture();
        sprite_texture(SDL_Texture* texture);
        sprite_texture(SDL_Texture* texture, const glm::vec2& size);

        [[nodiscard]] SDL_Texture* get_sdl_texture() const;
        void set_sdl_texture(SDL_Texture* texture);

        [[nodiscard]] glm::vec2 get_size() const;
        [[nodiscard]] SDL_FRect get_source_rect() const;
        [[nodiscard]] glm::vec2 get_origin() const;
        [[nodiscard]] float get_rotation() const;

        void set_texture(SDL_Texture* texture);
        void set_size(const glm::vec2& size);
        void set_source_rect(const SDL_FRect& rect);
        void set_origin(const glm::vec2& origin);
        void set_rotation(float rotation);

        [[nodiscard]] bool is_valid() const;

    private:
        void auto_size();

        SDL_Texture* m_sdl_texture;
        SDL_FRect m_source_rect;
        glm::vec2 m_size;
        glm::vec2 m_origin;
        float m_rotation;
    };

    inline SDL_Texture* sprite_texture::get_sdl_texture() const {
        return m_sdl_texture;
    }

    inline void sprite_texture::set_sdl_texture(SDL_Texture* texture) {
        m_sdl_texture = texture;
    }

    inline glm::vec2 sprite_texture::get_size() const {
        return m_size;
    }

    inline SDL_FRect sprite_texture::get_source_rect() const {
        return m_source_rect;
    }

    inline glm::vec2 sprite_texture::get_origin() const {
        return m_origin;
    }

    inline float sprite_texture::get_rotation() const {
        return m_rotation;
    }

    inline void sprite_texture::set_texture(SDL_Texture* texture) {
        m_sdl_texture = texture;
    }

    inline void sprite_texture::set_size(const glm::vec2& size) {
        m_size = size;
    }

    inline void sprite_texture::set_source_rect(const SDL_FRect& rect) {
        m_source_rect = rect;
    }

    inline void sprite_texture::set_origin(const glm::vec2& origin) {
        m_origin = origin;
    }

    inline void sprite_texture::set_rotation(float rotation) {
        m_rotation = rotation;
    }

    inline bool sprite_texture::is_valid() const {
        return m_sdl_texture != nullptr;
    }

    class sprite_texture_animated {};
}  // namespace engine
