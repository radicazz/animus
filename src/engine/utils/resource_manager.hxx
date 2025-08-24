#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#include <unordered_map>
#include <string>
#include <memory>

#include "../renderer/sprite.hxx"
#include "../renderer/font.hxx"

namespace engine {
    /**
     * @brief Manages the loading and unloading of game resources.
     *
     * @note Unloads all textures and fonts when the resource_manager is destroyed.
     *
     * @todo
     * - Add map of textures.
     * - Use textures to create sprites.
     * - Add font managing.
     *
     */
    class resource_manager {
    public:
        resource_manager(SDL_Renderer* renderer);
        ~resource_manager();

        resource_manager(const resource_manager&) = delete;
        resource_manager& operator=(const resource_manager&) = delete;

        game_sprite* sprite_load(std::string_view file_path);
        game_sprite* sprite_load(std::string_view file_path, const glm::vec2& size);
        void sprite_unload(std::string_view file_path);
        void sprite_unload(game_sprite& sprite);

        bool is_sprite_loaded(std::string_view file_path) const;

    private:
        SDL_Texture* texture_get_or_load(std::string_view file_path);
        void texture_unload(std::string_view file_path);
        bool is_texture_loaded(std::string_view file_path) const;

    private:
        std::unordered_map<std::string, std::unique_ptr<game_sprite>> m_sprites;
        std::unordered_map<std::string, SDL_Texture*> m_textures;

        // Reference to the renderer for handling textures.
        SDL_Renderer* m_sdl_renderer;
    };
}  // namespace engine
