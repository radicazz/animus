#pragma once

#include <glm/glm.hpp>

namespace engine {
    class game_camera {
    public:
        explicit game_camera(glm::vec2 world_position, float zoom_level);

        // Rule of 5 - using defaults since no resource management
        game_camera(const game_camera&) = default;
        game_camera& operator=(const game_camera&) = default;
        game_camera(game_camera&&) = default;
        game_camera& operator=(game_camera&&) = default;
        ~game_camera() = default;

        void set_position(const glm::vec2& position);
        [[nodiscard]] glm::vec2 get_position() const;
        void move_position(const glm::vec2& offset);

        void set_zoom(float zoom_level);
        [[nodiscard]] float get_zoom() const;
        void zoom_by(float factor);

        void set_physical_bounds(const glm::vec2& min_bounds, const glm::vec2& max_bounds);
        void clear_physical_bounds();
        [[nodiscard]] bool has_physical_bounds() const;
        [[nodiscard]] glm::vec2 get_physical_bounds_min() const;
        [[nodiscard]] glm::vec2 get_physical_bounds_max() const;

        void follow_target(const glm::vec2& target_position, float lerp_speed = 1.0f);
        void set_follow_offset(const glm::vec2& offset);

        // Called by a viewport to respect camera bounds, given visible half extents in world units.
        void clamp_to_physical_bounds(const glm::vec2& half_visible_world);

    private:
        glm::vec2 m_world_position;

        float m_zoom_level;
        static constexpr float min_zoom = 1.f;
        static constexpr float max_zoom = 2.f;

        // Camera bounds (optional)
        bool m_has_physical_bounds{false};
        glm::vec2 m_physical_bounds_min{0.0f};
        glm::vec2 m_physical_bounds_max{0.0f};

        // Following
        glm::vec2 m_follow_offset{0.0f};
    };

    inline void game_camera::set_position(const glm::vec2& position) {
        m_world_position = position;
    }

    inline glm::vec2 game_camera::get_position() const {
        return m_world_position;
    }

    inline void game_camera::move_position(const glm::vec2& offset) {
        m_world_position += offset;
    }

    inline void game_camera::set_zoom(float zoom_level) {
        m_zoom_level = glm::clamp(zoom_level, min_zoom, max_zoom);
    }

    inline float game_camera::get_zoom() const {
        return m_zoom_level;
    }

    inline void game_camera::zoom_by(float factor) {
        // Treat factor as an additive delta to the zoom level. This is more
        // intuitive for callers that pass small positive/negative values like
        // 0.2f / -0.2f to zoom in/out.
        set_zoom(m_zoom_level + factor);
    }

    inline void game_camera::set_follow_offset(const glm::vec2& offset) {
        m_follow_offset = offset;
    }

    inline bool game_camera::has_physical_bounds() const {
        return m_has_physical_bounds;
    }

}  // namespace engine
