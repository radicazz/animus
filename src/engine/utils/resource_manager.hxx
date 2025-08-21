#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#include <unordered_map>
#include <string>

namespace engine {
    /**
     * @brief Manages the loading and unloading of game resources.
     *
     * @note Unloads all textures and fonts when the resource_manager is destroyed.
     *
     */
    class resource_manager {
    public:
        resource_manager(SDL_Renderer* renderer);
        ~resource_manager();

        resource_manager(const resource_manager&) = delete;
        resource_manager& operator=(const resource_manager&) = delete;

        [[maybe_unused]] SDL_Texture* load_texture(std::string_view file_path);
        void unload_texture(std::string_view file_path);

        [[maybe_unused]] TTF_Font* load_font(std::string_view file_path, float size);
        void unload_font(std::string_view file_path);

        bool is_texture_loaded(std::string_view file_path) const;
        bool is_font_loaded(std::string_view file_path) const;

    private:
        std::unordered_map<std::string, SDL_Texture*> m_textures;
        std::unordered_map<std::string, TTF_Font*> m_fonts;

        // Reference to the renderer for handling textures.
        SDL_Renderer* m_sdl_renderer;
    };
}  // namespace engine
