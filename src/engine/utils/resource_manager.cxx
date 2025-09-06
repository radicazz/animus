#include "resource_manager.hxx"

#include "../renderer/renderer.hxx"

#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#include <stdexcept>
#include <format>

namespace engine {
    resource_manager::resource_manager(game_renderer& renderer) : m_renderer(renderer) {
    }

    resource_manager::~resource_manager() {
        textures_clear();
        fonts_clear();
    }

    std::unique_ptr<render_sprite> resource_manager::sprite_create(std::string_view file_path) {
        SDL_Texture* texture = texture_get_or_load(file_path);
        return std::make_unique<render_sprite>(file_path, texture);
    }

    std::unique_ptr<render_sprite> resource_manager::sprite_create(std::string_view file_path,
                                                                   const glm::vec2& size) {
        SDL_Texture* texture = texture_get_or_load(file_path);
        return std::make_unique<render_sprite>(file_path, texture, size);
    }

    std::unique_ptr<render_text> resource_manager::text_create(std::string_view font_path,
                                                               float font_size) {
        TTF_Font* font = font_get_or_load(font_path, font_size);
        constexpr std::string_view default_text = "Text";
        TTF_Text* text = TTF_CreateText(m_renderer.get_sdl_text_engine(), font, default_text.data(),
                                        default_text.length());
        return std::make_unique<render_text>(text);
    }

    void resource_manager::textures_clear() {
        for (auto& [key, texture] : m_textures) {
            SDL_DestroyTexture(texture);
            SDL_Log("Unloaded texture: %s", key.c_str());
        }

        m_textures.clear();
    }

    void resource_manager::fonts_clear() {
        for (auto& [key, font] : m_fonts) {
            TTF_CloseFont(font);
            SDL_Log("Unloaded font: %s", key.c_str());
        }

        m_fonts.clear();
    }

    SDL_Texture* resource_manager::texture_get_or_load(std::string_view file_path) {
        if (is_texture_loaded(file_path) == true) {
            return m_textures.at(file_path.data());
        }

        SDL_Texture* texture = IMG_LoadTexture(m_renderer.get_sdl_renderer(), file_path.data());
        if (texture == nullptr) {
            throw std::runtime_error(std::format("Failed to load the texture at: {}", file_path));
        }

        m_textures[file_path.data()] = texture;

        SDL_Log("Loaded texture: %s", file_path.data());

        return texture;
    }

    void resource_manager::texture_unload(std::string_view file_path) {
        auto it = m_textures.find(file_path.data());
        if (it != m_textures.end()) {
            SDL_DestroyTexture(it->second);
            m_textures.erase(it);
            SDL_Log("Unloaded texture: %s", file_path.data());
        }
    }

    bool resource_manager::is_texture_loaded(std::string_view file_path) const {
        return m_textures.find(file_path.data()) != m_textures.end();
    }

    TTF_Font* resource_manager::font_get_or_load(std::string_view font_path, float font_size) {
        std::string unique_key = font_create_unique_key(font_path, font_size);

        if (is_font_loaded(unique_key) == true) {
            return m_fonts.at(unique_key);
        }

        TTF_Font* font = TTF_OpenFont(font_path.data(), font_size);
        if (font == nullptr) {
            throw std::runtime_error(std::format("Failed to load font: {}", font_path));
        }

        m_fonts[unique_key] = font;

        SDL_Log("Loaded font: %s (size: %f)", font_path.data(), font_size);

        return font;
    }

    void resource_manager::font_unload(std::string_view unique_key) {
        auto it = m_fonts.find(unique_key.data());
        if (it != m_fonts.end()) {
            TTF_CloseFont(it->second);
            m_fonts.erase(it);
        }
    }

    bool resource_manager::is_font_loaded(std::string_view unique_key) const {
        return m_fonts.find(unique_key.data()) != m_fonts.end();
    }

    std::string resource_manager::font_create_unique_key(std::string_view font_path,
                                                         float font_size) const {
        return std::format("{}:{}", font_path, font_size);
    }

}  // namespace engine
