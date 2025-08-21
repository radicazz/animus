#include <glm/glm.hpp>

namespace engine {
    class camera {
    public:
        camera(glm::vec2 position, float zoom_level);

        void set_position(const glm::vec2& position);
        glm::vec2 get_position() const;
        void move_position(const glm::vec2& offset);

        void set_zoom(float zoom_level);
        float get_zoom() const;
        void zoom_by(float factor);

    private:
        glm::vec2 m_position;
        float m_zoom_level;

        static constexpr float min_zoom = 0.1f;
        static constexpr float max_zoom = 5.0f;
    };

    inline void camera::set_position(const glm::vec2& position) {
        m_position = position;
    }

    inline glm::vec2 camera::get_position() const {
        return m_position;
    }

    inline void camera::move_position(const glm::vec2& offset) {
        m_position += offset;
    }

    inline void camera::set_zoom(float zoom_level) {
        m_zoom_level = glm::clamp(zoom_level, min_zoom, max_zoom);
    }

    inline float camera::get_zoom() const {
        return m_zoom_level;
    }

    inline void camera::zoom_by(float factor) {
        set_zoom(m_zoom_level * factor);
    }

}  // namespace engine