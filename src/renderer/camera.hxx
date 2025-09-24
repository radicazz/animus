/**
 * @file camera.hxx
 * @brief 2D camera implementation header.
 */

#pragma once

#include <string>

#include <glm/glm.hpp>

namespace engine {
    class game_camera {
    public:
        game_camera() = delete;
        game_camera(std::string_view name, glm::vec2 position, float zoom);
        ~game_camera() = default;

        game_camera(const game_camera&) = default;
        game_camera& operator=(const game_camera&) = default;
        game_camera(game_camera&&) = default;
        game_camera& operator=(game_camera&&) = default;

        [[nodiscard]] std::string_view get_name() const;

        void set_position(const glm::vec2& position);
        [[nodiscard]] glm::vec2 get_position() const;
        void move_position(const glm::vec2& offset);

        void set_zoom(float zoom_level);
        [[nodiscard]] float get_zoom() const;
        void zoom_additive(float factor);
        void zoom_multiply(float factor);

        void set_physical_bounds(const glm::vec2& min_bounds, const glm::vec2& max_bounds);
        void clear_physical_bounds();
        [[nodiscard]] bool has_physical_bounds() const;
        [[nodiscard]] glm::vec2 get_physical_bounds_min() const;
        [[nodiscard]] glm::vec2 get_physical_bounds_max() const;

        void follow_target(const glm::vec2& target_position, float lerp_speed = 1.0f);
        void set_follow_offset(const glm::vec2& offset);

        // Called by a viewport to respect camera bounds, given visible half extents in world units.
        void clamp_to_physical_bounds(const glm::vec2& half_visible_world);

    public:
        static constexpr std::string_view default_name = "main";

    private:
        std::string m_name;

        glm::vec2 m_position;  ///< Camera position in world space.

        float m_zoom;
        static constexpr float min_zoom = 1.f;
        static constexpr float max_zoom = 2.f;

        // Camera bounds (optional)
        bool m_has_physical_bounds;
        glm::vec2 m_physical_bounds_min;
        glm::vec2 m_physical_bounds_max;

        glm::vec2 m_follow_offset;
    };

    inline std::string_view game_camera::get_name() const {
        return m_name;
    }

    inline void game_camera::set_position(const glm::vec2& position) {
        m_position = position;
    }

    inline glm::vec2 game_camera::get_position() const {
        return m_position;
    }

    inline void game_camera::move_position(const glm::vec2& offset) {
        m_position += offset;
    }

    inline void game_camera::set_zoom(float zoom_level) {
        m_zoom = glm::clamp(zoom_level, min_zoom, max_zoom);
    }

    inline float game_camera::get_zoom() const {
        return m_zoom;
    }

    inline void game_camera::zoom_additive(float factor) {
        set_zoom(m_zoom + factor);
    }

    inline void game_camera::zoom_multiply(float factor) {
        set_zoom(m_zoom * factor);
    }

    inline void game_camera::set_follow_offset(const glm::vec2& offset) {
        m_follow_offset = offset;
    }

    inline bool game_camera::has_physical_bounds() const {
        return m_has_physical_bounds;
    }

    inline glm::vec2 game_camera::get_physical_bounds_min() const {
        return m_physical_bounds_min;
    }

    inline glm::vec2 game_camera::get_physical_bounds_max() const {
        return m_physical_bounds_max;
    }
}  // namespace engine
