#include "renderer.hxx"

#include "camera.hxx"
#include "viewport.hxx"

#include "../logger.hxx"

#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>
#include <stdexcept>

namespace engine {
    game_renderer::game_renderer(SDL_Window* window)
        : m_sdl_renderer(nullptr),
          m_sdl_text_engine(nullptr),
          m_camera(nullptr),
          m_viewport(nullptr) {
        if (m_sdl_renderer = SDL_CreateRenderer(window, nullptr); m_sdl_renderer == nullptr) {
            TTF_Quit();
            SDL_Quit();
            throw std::runtime_error("Failed to create renderer");
        }

        log_info("Renderer created: {}", SDL_GetRendererName(m_sdl_renderer));

        m_sdl_text_engine = TTF_CreateRendererTextEngine(m_sdl_renderer);
        if (m_sdl_text_engine == nullptr) {
            SDL_DestroyRenderer(m_sdl_renderer);

            TTF_Quit();
            SDL_Quit();

            throw std::runtime_error("Failed to create TTF text engine.");
        }

        log_info("TTF text engine created successfully.");
    }

    game_renderer::~game_renderer() {
        if (m_sdl_text_engine != nullptr) {
            TTF_DestroyRendererTextEngine(m_sdl_text_engine);
            log_info("TTF text engine destroyed.");
        }

        if (m_sdl_renderer != nullptr) {
            SDL_DestroyRenderer(m_sdl_renderer);
            log_info("SDL Renderer destroyed.");
        }
    }

    game_renderer::game_renderer(game_renderer&& other) noexcept
        : m_sdl_renderer(other.m_sdl_renderer),
          m_sdl_text_engine(other.m_sdl_text_engine),
          m_camera(other.m_camera),
          m_viewport(other.m_viewport) {
        other.m_sdl_renderer = nullptr;
        other.m_sdl_text_engine = nullptr;
        other.m_camera = nullptr;
        other.m_viewport = nullptr;
    }

    game_renderer& game_renderer::operator=(game_renderer&& other) noexcept {
        if (this != &other) {
            // Clean up current resources
            if (m_sdl_text_engine) {
                TTF_DestroyRendererTextEngine(m_sdl_text_engine);
            }
            if (m_sdl_renderer) {
                SDL_DestroyRenderer(m_sdl_renderer);
            }

            // Move resources
            m_sdl_renderer = other.m_sdl_renderer;
            m_sdl_text_engine = other.m_sdl_text_engine;
            m_camera = other.m_camera;
            m_viewport = other.m_viewport;

            // Reset other
            other.m_sdl_renderer = nullptr;
            other.m_sdl_text_engine = nullptr;
            other.m_camera = nullptr;
            other.m_viewport = nullptr;
        }

        return *this;
    }

    void game_renderer::draw_begin() {
        if (m_viewport != nullptr) {
            m_viewport->apply_to_sdl(*this);
        } else {
            SDL_SetRenderViewport(m_sdl_renderer, nullptr);
        }

        SDL_SetRenderDrawColor(m_sdl_renderer, 0, 0, 0, 255);
        SDL_RenderClear(m_sdl_renderer);
    }

    void game_renderer::draw_end() {
        SDL_RenderPresent(m_sdl_renderer);
    }

    void game_renderer::sprite_draw_world(const game_sprite* sprite,
                                          const glm::vec2& world_position) {
        if (sprite == nullptr || sprite->is_valid() == false) {
            return;
        }

        glm::vec2 screen_position = world_position;

        if (m_camera != nullptr && m_viewport != nullptr) {
            // Transform via viewport + camera
            screen_position = m_viewport->world_to_screen(*m_camera, world_position);

            // Frustum culling
            if (m_viewport->is_in_view(*m_camera, world_position, sprite->get_size()) == false) {
                return;
            }
        }

        // Apply camera zoom to sprite size and origin
        glm::vec2 final_size = sprite->get_size();
        glm::vec2 final_origin = sprite->get_origin();
        glm::vec2 final_scale = sprite->get_scale();

        if (m_camera != nullptr) {
            float zoom = m_camera->get_zoom();
            final_size *= zoom;
            final_origin *= zoom;
        }

        final_size *= final_scale;

        const SDL_FRect dst_rect = {screen_position.x - final_origin.x,
                                    screen_position.y - final_origin.y, final_size.x, final_size.y};
        const SDL_FPoint center = {final_origin.x, final_origin.y};

        SDL_RenderTextureRotated(m_sdl_renderer, sprite->get_sdl_texture(), nullptr, &dst_rect,
                                 sprite->get_rotation(), &center, SDL_FLIP_NONE);
    }
    void game_renderer::sprite_draw_screen(const game_sprite* sprite,
                                           const glm::vec2& screen_position) {
        if (sprite == nullptr || sprite->is_valid() == false) {
            return;
        }

        const glm::vec2 size = sprite->get_size();
        const glm::vec2 origin = sprite->get_origin();

        const SDL_FRect dst_rect = {screen_position.x - origin.x, screen_position.y - origin.y,
                                    size.x, size.y};
        const SDL_FPoint center = {origin.x, origin.y};

        SDL_RenderTextureRotated(m_sdl_renderer, sprite->get_sdl_texture(), nullptr, &dst_rect,
                                 sprite->get_rotation(), &center, SDL_FLIP_NONE);
    }

    void game_renderer::text_draw_world(const game_text_dynamic* text,
                                        const glm::vec2& world_position) {
        if (text == nullptr || text->is_valid() == false) {
            return;
        }

        glm::vec2 screen_position;

        if (m_camera != nullptr) {
            // Transform world position to screen coordinates using camera
            screen_position = m_viewport->world_to_screen(*m_camera, world_position);

            // Frustum culling: skip drawing if text is outside view
            // Account for text scale and origin when calculating bounds
            const glm::vec2 text_size = text->get_size();
            const glm::vec2 text_scale = text->get_scale();
            const float camera_zoom = m_camera->get_zoom();
            const glm::vec2 total_scale = text_scale * camera_zoom;
            const glm::vec2 scaled_size = text_size * total_scale;

            if (m_viewport->is_in_view(*m_camera, world_position, scaled_size) == false) {
                return;
            }
        } else {
            screen_position = world_position;
        }

        // Delegate to screen drawing with camera zoom applied
        text_draw_screen(text, screen_position);
    }

    void game_renderer::text_draw_screen(const game_text_dynamic* text,
                                         const glm::vec2& screen_position) {
        if (text == nullptr || text->is_valid() == false) {
            return;
        }

        SDL_Texture* texture = text->get_sdl_texture();
        if (!texture) {
            return;
        }

        // Get text properties
        const glm::vec2 text_size = text->get_size();
        const glm::vec2 text_origin = text->get_origin();
        const glm::vec2 text_scale = text->get_scale();
        const float text_rotation = text->get_rotation();

        // Apply camera zoom to scale if camera is present
        glm::vec2 final_scale = text_scale;
        if (m_camera != nullptr) {
            final_scale *= m_camera->get_zoom();
        }

        // Calculate final size and position
        const glm::vec2 final_size = text_size * final_scale;
        const glm::vec2 scaled_origin = text_origin * final_scale;
        const glm::vec2 final_position = screen_position - scaled_origin;

        // Set up destination rectangle
        const SDL_FRect dest_rect = {final_position.x, final_position.y, final_size.x,
                                     final_size.y};

        // Set up rotation center point (relative to destination rectangle)
        const SDL_FPoint center = {scaled_origin.x, scaled_origin.y};

        // Render with transformations
        if (text_rotation != 0.0f) {
            // Render with rotation
            SDL_RenderTextureRotated(m_sdl_renderer, texture, nullptr, &dest_rect, text_rotation,
                                     &center, SDL_FLIP_NONE);
        } else {
            // Simple render without rotation (slightly more efficient)
            SDL_RenderTexture(m_sdl_renderer, texture, nullptr, &dest_rect);
        }
    }

    void game_renderer::text_draw_screen(const game_text_static* text,
                                         const glm::vec2& screen_position) {
        if (text == nullptr || text->is_valid() == false) {
            return;
        }

        // Account for origin.
        glm::vec2 adjusted_position = screen_position - text->get_origin();

        // For some reason this only works when rounded to nearest whole numbers... like ???
        adjusted_position = glm::floor(adjusted_position);

        TTF_DrawRendererText(text->get_sdl_text(), adjusted_position.x, adjusted_position.y);
    }

    glm::vec2 game_renderer::get_output_size() const {
        int w = 0, h = 0;
        SDL_GetRenderOutputSize(m_sdl_renderer, &w, &h);
        return {static_cast<float>(w), static_cast<float>(h)};
    }

    // Multi-viewport API implementation
    game_viewport& game_renderer::viewport_get_or_create(std::string_view name,
                                                         const glm::vec2& pos_norm,
                                                         const glm::vec2& size_norm) {
        auto it = m_viewports.find(std::string(name));
        if (it != m_viewports.end()) {
            return it->second;
        }
        game_viewport vp{size_norm};
        vp.set_normalized_position(pos_norm);
        auto [insert_it, _] = m_viewports.emplace(std::string(name), vp);
        // Maintain legacy pointer if first viewport or named "main" and none selected yet
        if (m_viewport == nullptr || name == "main") {
            m_viewport = &insert_it->second;
        }
        return insert_it->second;
    }

    game_viewport* game_renderer::viewport_get(std::string_view name) {
        auto it = m_viewports.find(std::string(name));
        if (it == m_viewports.end())
            return nullptr;
        return &it->second;
    }

    bool game_renderer::viewport_remove(std::string_view name) {
        auto it = m_viewports.find(std::string(name));
        if (it == m_viewports.end())
            return false;
        if (&it->second == m_viewport) {
            m_viewport = nullptr;  // legacy pointer invalidated
        }
        m_viewports.erase(it);
        return true;
    }

    game_viewport* game_renderer::viewport_main() {
        auto it = m_viewports.find("main");
        if (it == m_viewports.end())
            return nullptr;
        return &it->second;
    }

}  // namespace engine
