#include "resource_manager.hxx"

#include <stdexcept>
#include <format>

namespace engine {
    resource_manager::resource_manager(SDL_Renderer* renderer) : m_sdl_renderer(renderer) {
        if (m_sdl_renderer == nullptr) {
            throw std::runtime_error("Renderer cannot be null.");
        }

        if (TTF_Init() == false) {
            throw std::runtime_error("Failed to initialize SDL_ttf.");
        }
    }

    resource_manager::~resource_manager() {
        for (auto& [key, texture] : m_textures) {
            SDL_DestroyTexture(texture);
        }

        for (auto& [key, font] : m_fonts) {
            TTF_CloseFont(font);
        }

        TTF_Quit();
    }

    SDL_Texture* resource_manager::load_texture(std::string_view file_path) {
        // Check if already loaded.
        auto it = m_textures.find(file_path.data());
        if (it != m_textures.end()) {
            return it->second;
        }

        // Load the new texture.
        SDL_Texture* texture = IMG_LoadTexture(m_sdl_renderer, file_path.data());
        if (texture == nullptr) {
            throw std::runtime_error(std::format("Failed to load texture: {}", file_path));
        }

        m_textures[file_path.data()] = texture;
        return texture;
    }

    void resource_manager::unload_texture(std::string_view file_path) {
        auto it = m_textures.find(file_path.data());
        if (it != m_textures.end()) {
            SDL_DestroyTexture(it->second);
            m_textures.erase(it);
            SDL_Log("Unloaded texture: %s", file_path.data());
        }
    }

    [[maybe_unused]] TTF_Font* resource_manager::load_font(std::string_view file_path, float size) {
        // Check if already loaded.
        auto it = m_fonts.find(file_path.data());
        if (it != m_fonts.end()) {
            return it->second;
        }

        // Load the new font.
        TTF_Font* font = TTF_OpenFont(file_path.data(), size);
        if (font == nullptr) {
            throw std::runtime_error(std::format("Failed to load font: {}", file_path));
        }

        m_fonts[file_path.data()] = font;
        return font;
    }

    void resource_manager::unload_font(std::string_view file_path) {
        auto it = m_fonts.find(file_path.data());
        if (it != m_fonts.end()) {
            TTF_CloseFont(it->second);
            m_fonts.erase(it);
            SDL_Log("Unloaded font: %s", file_path.data());
        }
    }

    bool resource_manager::is_texture_loaded(std::string_view file_path) const {
        return m_textures.find(file_path.data()) != m_textures.end();
    }

    bool resource_manager::is_font_loaded(std::string_view file_path) const {
        return m_fonts.find(file_path.data()) != m_fonts.end();
    }

}  // namespace engine
