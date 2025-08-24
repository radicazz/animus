#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <string_view>

namespace engine {
    class game_font {
    public:
        game_font(std::string_view file_path, float size);
        ~game_font();

        [[nodiscard]] TTF_Font* get_sdl_font() const;

    private:
        TTF_Font* m_font;
    };

    inline TTF_Font* game_font::get_sdl_font() const {
        return m_font;
    }
}  // namespace engine
