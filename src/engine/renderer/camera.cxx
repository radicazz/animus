#include "camera.hxx"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace engine {
    game_camera::game_camera(glm::vec2 world_position, float zoom_level)
        : m_world_position(world_position) {
        m_zoom_level = glm::clamp(zoom_level, min_zoom, max_zoom);
    }

    void game_camera::set_physical_bounds(const glm::vec2& min_bounds,
                                          const glm::vec2& max_bounds) {
        m_physical_bounds_min = min_bounds;
        m_physical_bounds_max = max_bounds;
        m_has_physical_bounds = true;
    }

    void game_camera::clear_physical_bounds() {
        m_has_physical_bounds = false;
        m_physical_bounds_min = glm::vec2{0.0f};
        m_physical_bounds_max = glm::vec2{0.0f};
    }

    glm::vec2 game_camera::get_physical_bounds_min() const {
        return m_physical_bounds_min;
    }

    glm::vec2 game_camera::get_physical_bounds_max() const {
        return m_physical_bounds_max;
    }

    void game_camera::follow_target(const glm::vec2& target_position, float lerp_speed) {
        glm::vec2 desired_position = target_position + m_follow_offset;

        if (lerp_speed >= 1.0f) {
            set_position(desired_position);
        } else {
            glm::vec2 current_position = get_position();
            glm::vec2 new_position = glm::mix(current_position, desired_position, lerp_speed);
            set_position(new_position);
        }
    }

    void game_camera::clamp_to_physical_bounds(const glm::vec2& half_visible_world) {
        if (m_has_physical_bounds == false) {
            return;
        }

        const float min_x = m_physical_bounds_min.x + half_visible_world.x;
        const float max_x = m_physical_bounds_max.x - half_visible_world.x;
        const float min_y = m_physical_bounds_min.y + half_visible_world.y;
        const float max_y = m_physical_bounds_max.y - half_visible_world.y;

        if (min_x <= max_x) {
            m_world_position.x = glm::clamp(m_world_position.x, min_x, max_x);
        }
        if (min_y <= max_y) {
            m_world_position.y = glm::clamp(m_world_position.y, min_y, max_y);
        }
    }

}  // namespace engine
