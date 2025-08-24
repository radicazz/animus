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
        // Unload all textures.
        for (auto& [key, texture] : m_textures) {
            SDL_DestroyTexture(texture);
            SDL_Log("Unloaded texture: %s", key.c_str());
        }

        TTF_Quit();
    }

    game_sprite* resource_manager::sprite_load(std::string_view file_path) {
        // Create or access the texture.
        SDL_Texture* texture = texture_get_or_load(file_path);

        const std::string unique_key = std::format("{}_{}", file_path, m_sprites.size());

        auto sprite = std::make_unique<game_sprite>(file_path, texture);
        m_sprites[unique_key] = std::move(sprite);

        return m_sprites.at(unique_key).get();
    }

    game_sprite* resource_manager::sprite_load(std::string_view file_path, const glm::vec2& size) {
        SDL_Texture* texture = texture_get_or_load(file_path);

        const std::string unique_key = std::format("{}_{}", file_path, m_sprites.size());

        auto sprite = std::make_unique<game_sprite>(file_path, texture, size);
        m_sprites[unique_key] = std::move(sprite);

        return m_sprites.at(unique_key).get();
    }

    void resource_manager::sprite_unload(std::string_view file_path) {
        auto it = m_sprites.find(file_path.data());
        if (it != m_sprites.end()) {
            m_sprites.erase(it);
        }
    }

    void resource_manager::sprite_unload(game_sprite& sprite) {
        auto it = m_sprites.find(sprite.get_file_path().data());
        if (it != m_sprites.end()) {
            m_sprites.erase(it);
        }
    }

    bool resource_manager::is_sprite_loaded(std::string_view file_path) const {
        return m_sprites.find(file_path.data()) != m_sprites.end();
    }

    SDL_Texture* resource_manager::texture_get_or_load(std::string_view file_path) {
        if (is_texture_loaded(file_path) == true) {
            return m_textures.at(file_path.data());
        }

        SDL_Texture* texture = IMG_LoadTexture(m_sdl_renderer, file_path.data());
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
}  // namespace engine
