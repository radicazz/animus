#include "systems.hxx"

#include <glm/glm.hpp>
#include "../engine.hxx"

#include <algorithm>
#include <vector>
#include <cmath>

namespace engine {
    void system_physics::update(entt::registry& registry, float fixed_delta_time) {
        integrate_velocity(registry, fixed_delta_time);
    }

    void system_physics::integrate_velocity(entt::registry& registry, float delta_time) {
        auto view = registry.view<component_transform_interpolated, component_velocity>();
        for (auto entity : view) {
            auto& transform = view.get<component_transform_interpolated>(entity);
            const auto& velocity = view.get<component_velocity>(entity);

            // Store previous position for interpolation
            transform.previous_position = transform.position;
            transform.previous_rotation_degrees = transform.rotation_degrees;

            // Integrate position and rotation
            transform.position += velocity.linear * delta_time;
            transform.rotation_degrees += velocity.angular * delta_time;
        }
    }

    void system_renderer::render(entt::registry& registry, game_renderer& renderer,
                                 float interpolation_alpha) {
        render_sprites_interpolated(registry, renderer, interpolation_alpha);
    }

    void system_renderer::render_sprites_interpolated(entt::registry& registry,
                                                      game_renderer& renderer,
                                                      float interpolation_alpha) {
        auto view = registry.view<component_transform_interpolated, component_sprite>();

        std::vector<entt::entity> entities;
        entities.reserve(view.size_hint());

        for (auto entity : view) {
            const auto& sprite_comp = view.get<component_sprite>(entity);
            if (sprite_comp.sprite != nullptr) {
                entities.emplace_back(entity);
            }
        }

        std::sort(entities.begin(), entities.end(), [&](entt::entity a, entt::entity b) {
            const auto& sprite_a = registry.get<component_sprite>(a);
            const auto& sprite_b = registry.get<component_sprite>(b);
            return sprite_a.layer < sprite_b.layer;
        });

        // Render all entities
        for (auto entity : entities) {
            const auto& transform = registry.get<component_transform_interpolated>(entity);
            const auto& sprite_comp = registry.get<component_sprite>(entity);

            glm::vec2 interpolated_pos =
                glm::mix(transform.previous_position, transform.position, interpolation_alpha);

            float interpolated_rotation = glm::mix(transform.previous_rotation_degrees,
                                                   transform.rotation_degrees, interpolation_alpha);

            // Apply transform to sprite
            sprite_comp.sprite->set_rotation(interpolated_rotation);
            sprite_comp.sprite->set_scale(transform.scale);

            // Render sprite at world position
            renderer.sprite_draw_world(sprite_comp.sprite.get(), interpolated_pos);
        }
    }

    // Lifetime System Implementation
    void system_lifetime::update(entt::registry& registry, float delta_time) {
        auto view = registry.view<component_lifetime>();
        std::vector<entt::entity> entities_to_destroy;

        for (auto entity : view) {
            auto& lifetime = view.get<component_lifetime>(entity);
            lifetime.remaining_time_seconds -= delta_time;

            if (lifetime.remaining_time_seconds <= 0.0f) {
                entities_to_destroy.push_back(entity);
            }
        }

        // Destroy expired entities
        for (auto entity : entities_to_destroy) {
            registry.destroy(entity);
        }
    }
}  // namespace engine
