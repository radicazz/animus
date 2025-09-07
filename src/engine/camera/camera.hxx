#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace engine {
    class game_camera {
    public:
        game_camera(glm::vec2 world_position, float zoom_level);

        void set_position(const glm::vec2& position);
        [[nodiscard]] glm::vec2 get_position() const;
        void move_position(const glm::vec2& offset);

        void set_zoom(float zoom_level);
        [[nodiscard]] float get_zoom() const;
        void zoom_by(float factor);

        void set_viewport_size(const glm::vec2& size);
        [[nodiscard]] glm::vec2 get_viewport_size() const;

        [[nodiscard]] glm::mat3 get_view_matrix() const;
        [[nodiscard]] glm::vec2 world_to_screen(const glm::vec2& world_pos) const;
        [[nodiscard]] glm::vec2 screen_to_world(const glm::vec2& screen_pos) const;

        void set_physical_bounds(const glm::vec2& min_bounds, const glm::vec2& max_bounds);
        void clear_physical_bounds();
        [[nodiscard]] bool has_physical_bounds() const;
        [[nodiscard]] glm::vec2 get_physical_bounds_min() const;
        [[nodiscard]] glm::vec2 get_physical_bounds_max() const;

        void follow_target(const glm::vec2& target_position, float lerp_speed = 1.0f);
        void set_follow_offset(const glm::vec2& offset);

        [[nodiscard]] bool is_in_view(const glm::vec2& position, const glm::vec2& size) const;
        [[nodiscard]] std::tuple<glm::vec2, glm::vec2> get_visible_area() const;

    private:
        void clamp_to_physical_bounds();

    private:
        glm::vec2 m_world_position;

        float m_zoom_level;
        static constexpr float min_zoom = 1.f;
        static constexpr float max_zoom = 2.f;

        glm::vec2 m_viewport_size{800.0f, 600.0f};

        // Camera bounds (optional)
        bool m_has_physical_bounds{false};
        glm::vec2 m_physical_bounds_min{0.0f};
        glm::vec2 m_physical_bounds_max{0.0f};

        // Following
        glm::vec2 m_follow_offset{0.0f};
    };

    inline void game_camera::set_position(const glm::vec2& position) {
        m_world_position = position;
        clamp_to_physical_bounds();
    }

    inline glm::vec2 game_camera::get_position() const {
        return m_world_position;
    }

    inline void game_camera::move_position(const glm::vec2& offset) {
        m_world_position += offset;
        clamp_to_physical_bounds();
    }

    inline void game_camera::set_zoom(float zoom_level) {
        m_zoom_level = glm::clamp(zoom_level, min_zoom, max_zoom);
    }

    inline float game_camera::get_zoom() const {
        return m_zoom_level;
    }

    inline void game_camera::zoom_by(float factor) {
        set_zoom(m_zoom_level * factor);
    }

    inline void game_camera::set_viewport_size(const glm::vec2& size) {
        m_viewport_size = size;
    }

    inline glm::vec2 game_camera::get_viewport_size() const {
        return m_viewport_size;
    }

    inline void game_camera::set_follow_offset(const glm::vec2& offset) {
        m_follow_offset = offset;
    }

    inline bool game_camera::has_physical_bounds() const {
        return m_has_physical_bounds;
    }

}  // namespace engine
