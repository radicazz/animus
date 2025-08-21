#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <string>

namespace engine {
    class sprite {
    public:
        [[nodiscard]] SDL_Texture* get_sdl_texture() const;

    private:
        SDL_Texture* m_sdl_texture;
        SDL_FRect m_source_rect;

        glm::vec2 m_size;
        glm::vec2 m_origin;
    };

    inline SDL_Texture* sprite::get_sdl_texture() const {
        return m_sdl_texture;
    }

}  // namespace engine
