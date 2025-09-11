#include "resource_manager.hxx"

#include "../logger.hxx"
#include "../renderer/renderer.hxx"

#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#include <stdexcept>
#include <format>

namespace engine {
    game_resources::game_resources(game_renderer& renderer) : m_renderer(renderer) {
    }

    game_resources::~game_resources() {
        textures_clear();
        fonts_clear();
    }

    game_resources::game_resources(game_resources&& other) noexcept
        : m_textures(std::move(other.m_textures)),
          m_fonts(std::move(other.m_fonts)),
          m_renderer(other.m_renderer) {
        // Note: Can't move m_renderer as it's a reference
    }

    game_resources& game_resources::operator=(game_resources&& other) noexcept {
        if (this != &other) {
            // Clean up current resources
            textures_clear();
            fonts_clear();

            // Move resources (reference stays the same)
            m_textures = std::move(other.m_textures);
            m_fonts = std::move(other.m_fonts);
            // m_renderer reference can't be reassigned
        }
        return *this;
    }

    game_sprite::uptr game_resources::sprite_create(std::string_view file_path) {
        SDL_Texture* texture = texture_get_or_load(file_path);
        return std::make_unique<game_sprite>(file_path, texture);
    }

    game_sprite::uptr game_resources::sprite_create(std::string_view file_path,
                                                    const glm::vec2& size) {
        SDL_Texture* texture = texture_get_or_load(file_path);
        return std::make_unique<game_sprite>(file_path, texture, size);
    }

    game_text_static::uptr game_resources::text_create_static(std::string_view font_path,
                                                              float font_size) {
        TTF_Font* font = font_get_or_load(font_path, font_size);
        TTF_Text* text = TTF_CreateText(m_renderer.get_sdl_text_engine(), font,
                                        engine::default_text_content.data(),
                                        engine::default_text_content.length());
        return std::make_unique<game_text_static>(text);
    }

    game_text_dynamic::uptr game_resources::text_create_dynamic(std::string_view font_path,
                                                                float font_size) {
        TTF_Font* font = font_get_or_load(font_path, font_size);
        TTF_Text* text = TTF_CreateText(m_renderer.get_sdl_text_engine(), font,
                                        engine::default_text_content.data(),
                                        engine::default_text_content.length());
        auto static_text = std::make_unique<game_text_static>(text);
        return std::make_unique<game_text_dynamic>(engine::default_text_content,
                                                   std::move(static_text),
                                                   m_renderer.get_sdl_renderer(), font);
    }

    void game_resources::textures_clear() {
        for (auto& [key, texture] : m_textures) {
            SDL_DestroyTexture(texture);
            game_log("Unloaded texture: {}", key);
        }

        m_textures.clear();
    }

    void game_resources::fonts_clear() {
        for (auto& [key, font] : m_fonts) {
            TTF_CloseFont(font);
            game_log("Unloaded font: {}", key);
        }

        m_fonts.clear();
    }

    SDL_Texture* game_resources::texture_get_or_load(std::string_view file_path) {
        if (is_texture_loaded(file_path) == true) {
            game_log("Using cached texture: {}", file_path);
            return m_textures.at(file_path.data());
        }

        SDL_Texture* texture = IMG_LoadTexture(m_renderer.get_sdl_renderer(), file_path.data());
        if (texture == nullptr) {
            throw std::runtime_error(std::format("Failed to load the texture at: {}", file_path));
        }

        m_textures[file_path.data()] = texture;

        game_log("loaded texture: {}", file_path);

        return texture;
    }

    void game_resources::texture_unload(std::string_view file_path) {
        auto it = m_textures.find(file_path.data());
        if (it != m_textures.end()) {
            SDL_DestroyTexture(it->second);
            game_log("unloaded texture: {}", file_path);
            m_textures.erase(it);
        }
    }

    bool game_resources::is_texture_loaded(std::string_view file_path) const {
        return m_textures.contains(file_path.data());
    }

    TTF_Font* game_resources::font_get_or_load(std::string_view font_path, float font_size) {
        std::string unique_key = font_create_unique_key(font_path, font_size);

        if (is_font_loaded(unique_key) == true) {
            game_log("Using cached font: {}", unique_key);
            return m_fonts.at(unique_key);
        }

        TTF_Font* font = TTF_OpenFont(font_path.data(), font_size);
        if (font == nullptr) {
            throw std::runtime_error(std::format("Failed to load font: {}", font_path));
        }

        m_fonts[unique_key] = font;

        game_log("Loaded font: {} (size: {})", font_path, font_size);

        return font;
    }

    void game_resources::font_unload(std::string_view unique_key) {
        auto it = m_fonts.find(unique_key.data());
        if (it != m_fonts.end()) {
            TTF_CloseFont(it->second);
            game_log("Unloaded font: {}", unique_key);
            m_fonts.erase(it);
        }
    }

    bool game_resources::is_font_loaded(std::string_view unique_key) const {
        return m_fonts.contains(unique_key.data());
    }

    std::string game_resources::font_create_unique_key(std::string_view font_path,
                                                       float font_size) const {
        return std::format("{}:{}", font_path, font_size);
    }
}  // namespace engine
