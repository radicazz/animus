#pragma once

#include <unordered_map>
#include <string>
#include <string_view>
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
        explicit game_resources(game_renderer& renderer);
        ~game_resources();

        // Resource management - disable copy, enable move
        game_resources(const game_resources&) = delete;
        game_resources& operator=(const game_resources&) = delete;
        game_resources(game_resources&& other) noexcept;
        game_resources& operator=(game_resources&& other) noexcept;

        // Sprite resource management - cached sprites
        game_sprite* sprite_get_or_create(std::string_view key, std::string_view file_path);
        game_sprite* sprite_get(std::string_view key);
        void sprite_destroy(std::string_view key);

        game_text_static* text_static_get_or_create(std::string_view key, std::string_view text,
                                                    std::string_view font_path, float font_size);
        game_text_static* text_static_get(std::string_view key);
        void text_static_destroy(std::string_view key);

        game_text_dynamic* text_dynamic_get_or_create(std::string_view key,
                                                      std::string_view initial_text,
                                                      std::string_view font_path, float font_size);
        game_text_dynamic* text_dynamic_get(std::string_view key);
        void text_dynamic_destroy(std::string_view key);

        void textures_clear();
        void fonts_clear();
        void sprites_clear();
        void texts_clear();

    private:
        SDL_Texture* texture_get_or_create(std::string_view file_path);
        void texture_destroy(std::string_view file_path);
        bool is_texture_loaded(std::string_view file_path) const;

        TTF_Font* font_get_or_create(std::string_view font_path, float font_size);
        void font_destroy(std::string_view unique_key);
        bool is_font_loaded(std::string_view unique_key) const;
        std::string get_font_unique_key(std::string_view font_path, float font_size) const;

    private:
        std::unordered_map<std::string, SDL_Texture*> m_textures;
        std::unordered_map<std::string, game_sprite::uptr> m_sprites;

        std::unordered_map<std::string, TTF_Font*> m_fonts;
        std::unordered_map<std::string, game_text_static::uptr> m_static_texts;
        std::unordered_map<std::string, game_text_dynamic::uptr> m_dynamic_texts;

        const game_renderer& m_renderer;
    };
}  // namespace engine
