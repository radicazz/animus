#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "../renderer/sprite.hxx"

namespace engine {
    struct component_transform {
        // TODO: Add Z-axis support for 3D transforms?
        glm::vec2 position = {0.0f, 0.0f};
        float rotation_degrees = 0.0f;
        glm::vec2 scale = {1.0f, 1.0f};
    };

    struct component_velocity {
        glm::vec2 linear = {0.0f, 0.0f};
        float max_speed = 0.0f;
        float drag = {0.0f};  // Linear drag coefficient (0 = no drag, 1 = full stop)
    };

    /**
     * @brief Sprite rendering component
     */
    struct component_sprite {
        std::unique_ptr<render_sprite> sprite_image = {nullptr};
    };

    struct component_lifetime {
        float remaining_time = {1.0f};    // Time left in seconds
        bool destroy_on_expire = {true};  // Whether to destroy or just mark expired
    };

}  // namespace engine
