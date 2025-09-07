#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "../renderer/sprite.hxx"

namespace engine {
    /**
     * @brief A standard 2D transform component with the ability for interpolation.
     */
    struct component_transform_interpolated {
        glm::vec2 position = {0.0f, 0.0f};
        glm::vec2 previous_position = {0.0f, 0.0f};

        float rotation_degrees = 0.0f;
        float previous_rotation_degrees = 0.0f;

        glm::vec2 scale = {1.0f, 1.0f};
    };

    struct component_velocity {
        glm::vec2 linear = {0.0f, 0.0f};
        float max_speed = -1.f;
        float drag = 0.f;
    };

    struct component_sprite {
        std::unique_ptr<render_sprite> sprite = nullptr;
        int layer = 0;
    };

    /**
     * @brief Component that gives an entity a certain lifetime in seconds. When the time runs out,
     * the entity is destroyed.
     */
    struct component_lifetime {
        float remaining_time_seconds = 1.0f;
    };

}  // namespace engine
