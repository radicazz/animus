/**
 * @file viewport.hxx
 * @brief Viewport definition for frame rendering.
 *
 * A viewport defines a rectangular area of the window where rendering occurs. It is specified
 * in normalized coordinates (0.f to 1.f) relative to the window size. The viewport can be
 * used in conjunction with a camera to transform world coordinates to screen coordinates.
 */

#pragma once

#include <glm/glm.hpp>

struct SDL_Renderer;

namespace engine {
    class game_camera;
    class game_renderer;

    /**
     * @brief A rectangular render target in window space.
     */
    class game_viewport {
    public:
        game_viewport() = default;
        explicit game_viewport(const glm::vec2& size_normalized);
        ~game_viewport() = default;

        game_viewport(const game_viewport&) = default;
        game_viewport& operator=(const game_viewport&) = default;
        game_viewport(game_viewport&&) = default;
        game_viewport& operator=(game_viewport&&) = default;

        void set_normalized_position(const glm::vec2& new_position);
        void set_normalized_size(const glm::vec2& new_size);
        void set_normalized_rect(const glm::vec2& new_position, const glm::vec2& new_size);

        [[nodiscard]] glm::vec2 get_position_normalized() const;
        [[nodiscard]] glm::vec2 get_size_normalized() const;

        /**
         * Get last computed pixel position. Only valid after apply_to_sdl has been
         * called for the current frame (or after explicit compute_pixel_rect call).
         */
        [[nodiscard]] glm::vec2 get_position_pixels() const;
        [[nodiscard]] glm::vec2 get_size_pixels() const;

        /** Compute and apply viewport to SDL, given the renderer output size. */
        void apply_to_sdl(game_renderer& renderer) const;

        // Transform helpers using the camera (use cached pixel values).
        glm::mat3 get_view_matrix(const game_camera& camera) const;
        glm::vec2 world_to_screen(const game_camera& camera, const glm::vec2& world_pos) const;
        glm::vec2 screen_to_world(const game_camera& camera, const glm::vec2& screen_pos) const;

        std::tuple<glm::vec2, glm::vec2> get_visible_area_world(const game_camera& camera) const;
        bool is_in_view(const game_camera& camera, const glm::vec2& position,
                        const glm::vec2& size) const;

        void clamp_camera_to_bounds(game_camera& camera) const;

    private:
        static glm::vec2 clamp_normalized(const glm::vec2& vec);

    private:
        /**
         * @brief Viewport position normalized (0.f to 1.f) on x & y.
         */
        glm::vec2 m_position;

        /**
         * @brief Viewport size normalized  (0.f to 1.f) on x & y.
         */
        glm::vec2 m_size;

        mutable glm::vec2 m_cached_position_pixels;
        mutable glm::vec2 m_cached_size_pixels;
    };

    inline void game_viewport::set_normalized_position(const glm::vec2& new_position) {
        m_position = clamp_normalized(new_position);
    }

    inline void game_viewport::set_normalized_size(const glm::vec2& new_size) {
        m_size = clamp_normalized(new_size);
    }

    inline glm::vec2 game_viewport::get_position_normalized() const {
        return m_position;
    }

    inline glm::vec2 game_viewport::get_size_normalized() const {
        return m_size;
    }

    inline glm::vec2 game_viewport::get_position_pixels() const {
        return m_cached_position_pixels;
    }

    inline glm::vec2 game_viewport::get_size_pixels() const {
        return m_cached_size_pixels;
    }

    inline glm::vec2 game_viewport::clamp_normalized(const glm::vec2& vec) {
        return glm::clamp(vec, {0.f}, {1.f});
    }
}  // namespace engine
