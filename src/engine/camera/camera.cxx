#include "camera.hxx"

namespace engine {
    camera::camera(glm::vec2 position, float zoom_level) : m_position(position) {
        m_zoom_level = glm::clamp(zoom_level, min_zoom, max_zoom);
    }

}  // namespace engine