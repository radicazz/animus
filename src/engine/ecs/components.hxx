#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "../renderer/sprite.hxx"

namespace engine {
    struct component_transform {
        glm::vec2 position = {0.0f, 0.0f};
        float rotation_degrees = 0.0f;
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

    struct component_lifetime {
        float remaining_time = 1.0f;
        bool destroy_on_expire = true;
    };

}  // namespace engine
