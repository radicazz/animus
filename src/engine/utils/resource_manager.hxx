#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "../renderer/sprite.hxx"
#include "../renderer/text.hxx"

namespace engine {
    class game_renderer;

    /**
     * @brief Manages the loading and unloading of game resources for the engine.
     */
    class resource_manager {
    public:
        resource_manager(game_renderer& renderer);
        ~resource_manager();

        resource_manager(const resource_manager&) = delete;
        resource_manager& operator=(const resource_manager&) = delete;

        std::unique_ptr<render_sprite> sprite_create(std::string_view file_path);
        std::unique_ptr<render_sprite> sprite_create(std::string_view file_path,
                                                     const glm::vec2& size);

        std::unique_ptr<render_text> text_create(std::string_view font_path, float font_size);

        void textures_clear();
        void fonts_clear();

    private:
        SDL_Texture* texture_get_or_load(std::string_view file_path);
        void texture_unload(std::string_view file_path);
        bool is_texture_loaded(std::string_view file_path) const;

        TTF_Font* font_get_or_load(std::string_view font_path, float font_size);
        void font_unload(std::string_view unique_key);
        bool is_font_loaded(std::string_view unique_key) const;
        std::string font_create_unique_key(std::string_view font_path, float font_size) const;

    private:
        std::unordered_map<std::string, SDL_Texture*> m_textures;
        std::unordered_map<std::string, TTF_Font*> m_fonts;

        const game_renderer& m_renderer;
    };
}  // namespace engine
