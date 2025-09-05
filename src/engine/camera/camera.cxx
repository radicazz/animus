#include "camera.hxx"
#include <algorithm>

namespace engine {
    camera::camera(glm::vec2 world_position, float zoom_level) : m_world_position(world_position) {
        m_zoom_level = glm::clamp(zoom_level, min_zoom, max_zoom);
    }

    glm::mat3 camera::get_view_matrix() const {
        // Create a single transformation matrix that handles camera positioning and zoom
        // For 2D camera: we want to transform world coordinates to screen coordinates
        // The transformation should center the camera position on screen and apply zoom
        
        glm::mat3 view_matrix = glm::mat3(1.0f);
        
        // Apply zoom (scale)
        view_matrix[0][0] = m_zoom_level;
        view_matrix[1][1] = m_zoom_level;
        
        // Apply translation to center the camera position on screen
        // Formula: screen_pos = (world_pos - camera_pos) * zoom + screen_center
        view_matrix[2][0] = -m_world_position.x * m_zoom_level + m_viewport_size.x * 0.5f;
        view_matrix[2][1] = -m_world_position.y * m_zoom_level + m_viewport_size.y * 0.5f;
        
        return view_matrix;
    }

    glm::vec2 camera::world_to_screen(const glm::vec2& world_pos) const {
        // Transform world position to screen coordinates
        glm::vec3 world_homogeneous = glm::vec3(world_pos, 1.0f);
        glm::vec3 screen_homogeneous = get_view_matrix() * world_homogeneous;
        return glm::vec2(screen_homogeneous.x, screen_homogeneous.y);
    }

    glm::vec2 camera::screen_to_world(const glm::vec2& screen_pos) const {
        // Transform screen position to world coordinates
        glm::vec2 centered_screen = screen_pos - m_viewport_size * 0.5f;
        return m_world_position + centered_screen / m_zoom_level;
    }

    void camera::set_bounds(const glm::vec2& min_bounds, const glm::vec2& max_bounds) {
        m_bounds_min = min_bounds;
        m_bounds_max = max_bounds;
        m_has_bounds = true;
        clamp_to_bounds();
    }

    void camera::clear_bounds() {
        m_has_bounds = false;
    }

    glm::vec2 camera::get_bounds_min() const {
        return m_bounds_min;
    }

    glm::vec2 camera::get_bounds_max() const {
        return m_bounds_max;
    }

    void camera::follow_target(const glm::vec2& target_position, float lerp_speed) {
        glm::vec2 desired_position = target_position + m_follow_offset;
        
        if (lerp_speed >= 1.0f) {
            // Instant following
            set_position(desired_position);
        } else {
            // Smooth following with lerp
            glm::vec2 current_position = get_position();
            glm::vec2 new_position = glm::mix(current_position, desired_position, lerp_speed);
            set_position(new_position);
        }
    }

    bool camera::is_in_view(const glm::vec2& position, const glm::vec2& size) const {
        glm::vec4 view_bounds = get_view_bounds();
        
        // Check if object bounds intersect with view bounds
        float obj_left = position.x - size.x * 0.5f;
        float obj_right = position.x + size.x * 0.5f;
        float obj_top = position.y - size.y * 0.5f;
        float obj_bottom = position.y + size.y * 0.5f;

        return !(obj_right < view_bounds.x ||   // Object is to the left of view
                 obj_left > view_bounds.z ||    // Object is to the right of view
                 obj_bottom < view_bounds.y ||  // Object is above view
                 obj_top > view_bounds.w);      // Object is below view
    }

    glm::vec4 camera::get_view_bounds() const {
        // Calculate the world bounds of what the camera can see
        glm::vec2 half_viewport = m_viewport_size * 0.5f / m_zoom_level;
        
        float min_x = m_world_position.x - half_viewport.x;
        float min_y = m_world_position.y - half_viewport.y;
        float max_x = m_world_position.x + half_viewport.x;
        float max_y = m_world_position.y + half_viewport.y;

        return glm::vec4(min_x, min_y, max_x, max_y);
    }

    void camera::clamp_to_bounds() {
        if (m_has_bounds == false) {
            return;
        }

        // Calculate the camera's view area in world space
        glm::vec2 half_viewport = m_viewport_size * 0.5f / m_zoom_level;
        
        // Clamp camera position to ensure it doesn't go outside bounds
        float min_x = m_bounds_min.x + half_viewport.x;
        float max_x = m_bounds_max.x - half_viewport.x;
        float min_y = m_bounds_min.y + half_viewport.y;
        float max_y = m_bounds_max.y - half_viewport.y;

        // Only clamp if the bounds are large enough to contain the viewport
        if (min_x <= max_x) {
            m_world_position.x = glm::clamp(m_world_position.x, min_x, max_x);
        }
        if (min_y <= max_y) {
            m_world_position.y = glm::clamp(m_world_position.y, min_y, max_y);
        }
    }

}  // namespace engine
