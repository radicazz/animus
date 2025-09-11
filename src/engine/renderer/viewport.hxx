#pragma once

#include <glm/glm.hpp>

struct SDL_Renderer;

namespace engine {

    class game_camera;

    // A rectangular render target in window space.
    // Responsible for world<->screen transforms and culling, given a camera.
    class game_viewport {
    public:
        game_viewport() = default;
        explicit game_viewport(const glm::vec2& size_pixels)
            : m_position_pixels(0.0f), m_size_pixels(size_pixels) {
        }

        game_viewport(const glm::vec2& position_pixels, const glm::vec2& size_pixels)
            : m_position_pixels(position_pixels), m_size_pixels(size_pixels) {
        }

        // Rule of 5 - using defaults since no resource management
        game_viewport(const game_viewport&) = default;
        game_viewport& operator=(const game_viewport&) = default;
        game_viewport(game_viewport&&) = default;
        game_viewport& operator=(game_viewport&&) = default;
        ~game_viewport() = default;

        void set_position_pixels(const glm::vec2& pos) {
            m_position_pixels = pos;
        }

        void set_size_pixels(const glm::vec2& size) {
            m_size_pixels = size;
        }

        void set_rect_pixels(const glm::vec2& pos, const glm::vec2& size) {
            m_position_pixels = pos;
            m_size_pixels = size;
        }

        glm::vec2 get_position_pixels() const {
            return m_position_pixels;
        }

        glm::vec2 get_size_pixels() const {
            return m_size_pixels;
        }

        // Apply the viewport to the SDL renderer (sets SDL_SetRenderViewport).
        void apply_to_sdl(SDL_Renderer* sdl_renderer) const;

        // Transform helpers using the camera.
        glm::mat3 get_view_matrix(const game_camera& camera) const;
        glm::vec2 world_to_screen(const game_camera& camera, const glm::vec2& world_pos) const;
        glm::vec2 screen_to_world(const game_camera& camera, const glm::vec2& screen_pos) const;

        // Culling helpers
        std::tuple<glm::vec2, glm::vec2> get_visible_area_world(const game_camera& camera) const;
        bool is_in_view(const game_camera& camera, const glm::vec2& position,
                        const glm::vec2& size) const;

        // Clamp the camera to its physical bounds (if any), using this viewport’s visible extents.
        void clamp_camera_to_bounds(game_camera& camera) const;

    private:
        glm::vec2 m_position_pixels{0.0f};  // top-left in window coordinates
        glm::vec2 m_size_pixels{800.0f, 600.0f};
    };

}  // namespace engine
