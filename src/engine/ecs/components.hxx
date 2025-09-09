#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "../renderer/sprite.hxx"

namespace engine {
    struct component_sprite_single {
        std::unique_ptr<game_sprite> sprite = nullptr;
    };

    struct component_renderable {
        bool is_visible = true;
        int layer = 0;
    };

    struct component_transform {
        glm::vec2 position = {0.0f, 0.0f};
        float rotation = 0.0f;
        glm::vec2 scale = {1.0f, 1.0f};
    };

    struct component_interpolation {
        glm::vec2 previous_position = {0.0f, 0.0f};
        float previous_rotation = 0.0f;
    };

    struct component_velocity_linear {
        glm::vec2 value = {0.0f, 0.0f};
        float max_speed = 1000.0f;
        float drag = 0.0f;
    };

    struct component_velocity_angular {
        float value = 0.0f;
        float max_speed = 360.0f;  // degrees per second
        float drag = 0.0f;
    };

    struct component_lifetime {
        float remaining_seconds = 5.f;
    };
}  // namespace engine
