#include "systems.hxx"

#include <glm/glm.hpp>
#include "../engine.hxx"

#include <algorithm>
#include <vector>
#include <cmath>

namespace engine {
    void system_physics::tick(entt::registry& registry, float delta_time) {
        // First save previous state for interpolation next frame.
        auto interp_view = registry.view<component_transform, component_interpolation>();
        for (auto [entity, transform, interp] : interp_view.each()) {
            interp.previous_position = transform.position;
            interp.previous_rotation = transform.rotation;
        }

        // Apply linear velocity.
        auto linear_view = registry.view<component_transform, component_velocity_linear>();
        for (auto [entity, transform, velocity] : linear_view.each()) {
            glm::vec2 vel = velocity.value;

            // Apply drag
            if (velocity.drag > 0.0f) {
                float drag_factor = 1.0f - (velocity.drag * delta_time);
                vel *= std::max(0.0f, drag_factor);
                velocity.value = vel;
            }

            // Apply max speed limit
            if (velocity.max_speed > 0.0f) {
                float speed = glm::length(vel);
                if (speed > velocity.max_speed) {
                    vel = (vel / speed) * velocity.max_speed;
                    velocity.value = vel;
                }
            }

            // Apply velocity to position
            transform.position += vel * delta_time;
        }

        // Apply angular velocity
        auto angular_view = registry.view<component_transform, component_velocity_angular>();
        for (auto [entity, transform, velocity] : angular_view.each()) {
            float vel = velocity.value;

            // Apply drag
            if (velocity.drag > 0.0f) {
                float drag_factor = 1.0f - (velocity.drag * delta_time);
                vel *= std::max(0.0f, drag_factor);
                velocity.value = vel;
            }

            // Apply max speed limit
            if (velocity.max_speed > 0.0f) {
                if (std::abs(vel) > velocity.max_speed) {
                    vel = std::copysign(velocity.max_speed, vel);
                    velocity.value = vel;
                }
            }

            // Apply velocity to rotation
            transform.rotation += vel * delta_time;

            // Normalize rotation
            while (transform.rotation >= 360.0f) {
                transform.rotation -= 360.0f;
            }

            while (transform.rotation < 0.0f) {
                transform.rotation += 360.0f;
            }
        }
    }

    void system_renderer::tick(entt::registry& registry, game_renderer& renderer,
                               float interpolation_alpha) {
        struct render_data {
            entt::entity entity;
            int layer;
            const component_transform* transform;
            const component_sprite_single* sprite;
            const component_interpolation* interp;
        };

        auto view =
            registry.view<component_transform, component_renderable, component_sprite_single>();

        std::vector<render_data> render_queue;
        render_queue.reserve(view.size_hint());

        for (auto [entity, transform, renderable, sprite] : view.each()) {
            if (renderable.is_visible == true) {
                render_queue.emplace_back(
                    render_data{entity, renderable.layer, &transform, &sprite,
                                registry.try_get<component_interpolation>(entity)});
            }
        }

        // Sort sprites by layer.
        std::ranges::sort(render_queue,
                          [](const auto& a, const auto& b) { return a.layer < b.layer; });

        for (const auto& data : render_queue) {
            if (data.sprite->sprite == nullptr) {
                continue;
            }

            glm::vec2 render_position = data.transform->position;
            float render_rotation = data.transform->rotation;

            if (data.interp != nullptr) {
                // Interpolate the sprite's postion.
                render_position = glm::mix(data.interp->previous_position, data.transform->position,
                                           interpolation_alpha);

                // Interpolate the sprite's rotation, taking into account wrap-around at 360
                // degrees.
                const float previous_rotation = data.interp->previous_rotation;
                const float current_rotation = data.transform->rotation;

                float rotation_diff = current_rotation - previous_rotation;
                if (rotation_diff > 180.0f) {
                    rotation_diff -= 360.0f;
                } else if (rotation_diff < -180.0f) {
                    rotation_diff += 360.0f;
                }

                render_rotation = previous_rotation + (rotation_diff * interpolation_alpha);

                while (render_rotation >= 360.0f) {
                    render_rotation -= 360.0f;
                }

                while (render_rotation < 0.0f) {
                    render_rotation += 360.0f;
                }
            }

            data.sprite->sprite->set_rotation(render_rotation);
            data.sprite->sprite->set_scale(data.transform->scale);

            renderer.sprite_draw_world(data.sprite->sprite, render_position);
        }
    }

    // Lifetime System Implementation
    void system_lifetime::tick(entt::registry& registry, float delta_time) {
        auto view = registry.view<component_lifetime>();
        std::vector<entt::entity> entities_to_destroy;

        for (auto [entity, lifetime] : view.each()) {
            lifetime.remaining_seconds -= delta_time;

            if (lifetime.remaining_seconds <= 0.0f) {
                entities_to_destroy.push_back(entity);
            }
        }

        // Destroy expired entities.
        for (auto entity : entities_to_destroy) {
            registry.destroy(entity);
        }
    }
}  // namespace engine
