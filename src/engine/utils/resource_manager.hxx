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
     *
     * This class handles loading, caching, and unloading of textures and fonts used in a game. It
     * holds an internal list of loaded resources to avoid redundant loading and provides methods to
     * create sprites and text objects. When creating a sprite or a font, the manager will check if
     * the resource is already loaded and reuse it if available. The manager also provides methods
     * to unload specific resources or clear all loaded resources manually.
     */
    class game_resources {
    public:
        game_resources(game_renderer& renderer);
        ~game_resources();

        game_resources(const game_resources&) = delete;
        game_resources& operator=(const game_resources&) = delete;
        game_resources(game_resources&&) = delete;
        game_resources& operator=(game_resources&&) = delete;

        /**
         * @brief Creates a sprite from an image texture file.
         * @param file_path The path to the texture file.
         * @return A unique pointer to the created sprite.
         */
        game_sprite::uptr sprite_create(std::string_view file_path);

        /**
         * @brief Creates a sprite from an image texture file with a specified size.
         * @param file_path The path to the texture file.
         * @param size The desired size of the sprite.
         * @return A unique pointer to the created sprite.
         */
        game_sprite::uptr sprite_create(std::string_view file_path, const glm::vec2& size);

        /**
         * @brief Creates a static text object from a font file.
         * @param font_path The path to the font file.
         * @param font_size The size of the font.
         * @return A unique pointer to the created static text object.
         */
        game_text_static::uptr text_create_static(std::string_view font_path, float font_size);

        /**
         * @brief Creates a dynamic text object from a font file.
         * @param font_path The path to the font file.
         * @param font_size The size of the font.
         * @return A unique pointer to the created dynamic text object.
         */
        std::unique_ptr<render_text_dynamic> text_create_dynamic(std::string_view font_path,
                                                                 float font_size);

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
