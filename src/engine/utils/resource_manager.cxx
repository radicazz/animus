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
        for (auto& [key, sprite] : m_sprites) {
            SDL_DestroyTexture(sprite->get_sdl_texture());
        }

        for (auto& [key, font] : m_fonts) {
            TTF_CloseFont(font.get_sdl_font());
        }

        TTF_Quit();
    }

    game_sprite* resource_manager::sprite_load(std::string_view file_path) {
        if (is_sprite_loaded(file_path)) {
            SDL_Log("Sprite already loaded: %s", file_path.data());
            return m_sprites[file_path.data()].get();
        }

        auto sprite = std::make_unique<game_sprite>(file_path, m_sdl_renderer);
        m_sprites[file_path.data()] = std::move(sprite);

        SDL_Log("Loaded texture: %s", file_path.data());

        return sprite.get();
    }

    game_sprite* resource_manager::sprite_load(std::string_view file_path, const glm::vec2& size) {
        if (is_sprite_loaded(file_path)) {
            SDL_Log("Sprite already loaded: %s", file_path.data());
            return m_sprites.at(file_path.data()).get();
        }

        auto sprite = std::make_unique<game_sprite>(file_path, m_sdl_renderer, size);
        m_sprites[file_path.data()] = std::move(sprite);

        SDL_Log("Loaded texture: %s", file_path.data());

        return sprite.get();
    }

    void resource_manager::sprite_unload(std::string_view file_path) {
        auto it = m_sprites.find(file_path.data());
        if (it != m_sprites.end()) {
            it->second->unload();
            it->second.reset();
            m_sprites.erase(it);

            SDL_Log("Unloaded texture: %s", file_path.data());
        }
    }

    void resource_manager::sprite_unload(game_sprite& sprite) {
        auto it = m_sprites.find(sprite.get_file_path().data());
        if (it != m_sprites.end()) {
            it->second->unload();
            it->second.reset();
            m_sprites.erase(it);

            SDL_Log("Unloaded texture: %s", sprite.get_file_path().data());
        }
    }

    game_sprite* resource_manager::sprite_get(std::string_view file_path) {
        auto it = m_sprites.find(file_path.data());
        if (it != m_sprites.end()) {
            return it->second.get();
        }

        throw std::runtime_error(std::format("Sprite not found: {}", file_path));
    }

    bool resource_manager::is_sprite_loaded(std::string_view file_path) const {
        return m_sprites.find(file_path.data()) != m_sprites.end();
    }

    bool resource_manager::is_font_loaded(std::string_view file_path) const {
        return m_fonts.find(file_path.data()) != m_fonts.end();
    }
}  // namespace engine
