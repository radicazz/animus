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
        auto view =
            registry.view<component_transform, component_renderable, component_sprite_single>();

        std::vector<entt::entity> entities;
        entities.reserve(view.size_hint());

        for (auto entity : view) {
            const auto& renderable = registry.get<component_renderable>(entity);
            if (renderable.is_visible == true) {
                entities.push_back(entity);
            }
        }

        std::sort(entities.begin(), entities.end(), [&](entt::entity a, entt::entity b) {
            const auto& sprite_a = registry.get<component_renderable>(a);
            const auto& sprite_b = registry.get<component_renderable>(b);
            return sprite_a.layer < sprite_b.layer;
        });

        for (auto entity : entities) {
            const auto& transform = registry.get<component_transform>(entity);
            const auto& sprite_comp = registry.get<component_sprite_single>(entity);

            glm::vec2 render_position = transform.position;
            float render_rotation = transform.rotation;

            // Use interpolation if available,
            if (const auto* interp = registry.try_get<component_interpolation>(entity)) {
                render_position =
                    glm::mix(interp->previous_position, transform.position, interpolation_alpha);
                render_rotation =
                    glm::mix(interp->previous_rotation, transform.rotation, interpolation_alpha);
            }

            // Apply transform to sprite
            sprite_comp.sprite->set_rotation(render_rotation);
            sprite_comp.sprite->set_scale(transform.scale);

            // Render sprite at world position
            renderer.sprite_draw_world(sprite_comp.sprite, render_position);
        }
    }

    // Lifetime System Implementation
    void system_lifetime::tick(entt::registry& registry, float delta_time) {
        auto view = registry.view<component_lifetime_destroy>();
        std::vector<entt::entity> entities_to_destroy;

        for (auto entity : view) {
            auto& lifetime = view.get<component_lifetime_destroy>(entity);
            lifetime.remaining_seconds -= delta_time;

            if (lifetime.remaining_seconds <= 0.0f) {
                entities_to_destroy.push_back(entity);
            }
        }

        // Destroy expired entities
        for (auto entity : entities_to_destroy) {
            registry.destroy(entity);
        }
    }
}  // namespace engine
