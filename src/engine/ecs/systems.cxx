#include "systems.hxx"

#include <glm/glm.hpp>
#include "../engine.hxx"

#include <algorithm>
#include <vector>
#include <cmath>

namespace engine {
    void system_physics::update(entt::registry& registry, float delta_time) {
        apply_drag(registry, delta_time);
        enforce_speed_limits(registry);
        integrate_velocity(registry, delta_time);
    }

    void system_physics::apply_drag(entt::registry& registry, float delta_time) {
        auto view = registry.view<component_velocity>();
        for (auto entity : view) {
            auto& vel = view.get<component_velocity>(entity);

            if (vel.drag > 0.0f) {
                // Apply exponential drag
                float drag_factor = std::pow(1.0f - vel.drag, delta_time);
                vel.linear *= drag_factor;
            }
        }
    }

    void system_physics::enforce_speed_limits(entt::registry& registry) {
        auto view = registry.view<component_velocity>();
        for (auto entity : view) {
            auto& vel = view.get<component_velocity>(entity);

            if (vel.max_speed > 0.0f) {
                float current_speed = glm::length(vel.linear);
                if (current_speed > vel.max_speed) {
                    vel.linear = glm::normalize(vel.linear) * vel.max_speed;
                }
            }
        }
    }

    void system_physics::integrate_velocity(entt::registry& registry, float delta_time) {
        auto view = registry.view<component_transform_interpolated, component_velocity>();
        for (auto entity : view) {
            auto& transform = view.get<component_transform_interpolated>(entity);
            const auto& velocity = view.get<component_velocity>(entity);

            // Update position based on velocity
            transform.position += velocity.linear * delta_time;
        }
    }

    // Renderer System Implementation
    void system_renderer::render(entt::registry& registry, game_engine* engine,
                                 float interpolation_alpha) {
        auto view = registry.view<component_transform_interpolated, component_sprite>();
        auto& renderer = engine->get_renderer();

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

    // ECS Utils Implementation
    entt::entity ecs_utils::create_sprite_entity(entt::registry& registry,
                                                 const glm::vec2& position,
                                                 std::unique_ptr<render_sprite> sprite) {
        auto entity = registry.create();

        // Add transform component
        registry.emplace<component_transform_interpolated>(entity, position, position, 0.0f, 0.0f,
                                                           glm::vec2{1.0f, 1.0f});

        // Add sprite component
        registry.emplace<component_sprite>(entity, std::move(sprite));

        return entity;
    }

    void ecs_utils::set_position(entt::registry& registry, entt::entity entity,
                                 const glm::vec2& position) {
        if (auto* transform = registry.try_get<component_transform_interpolated>(entity)) {
            transform->position = position;
        }
    }

    glm::vec2 ecs_utils::get_position(entt::registry& registry, entt::entity entity) {
        if (auto* transform = registry.try_get<component_transform_interpolated>(entity)) {
            return transform->position;
        }

        return {0.0f, 0.0f};
    }

    void ecs_utils::set_rotation(entt::registry& registry, entt::entity entity,
                                 float rotation_degrees) {
        if (auto* transform = registry.try_get<component_transform_interpolated>(entity)) {
            transform->rotation_degrees = rotation_degrees;
        }
    }

    float ecs_utils::get_rotation(entt::registry& registry, entt::entity entity) {
        if (auto* transform = registry.try_get<component_transform_interpolated>(entity)) {
            return transform->rotation_degrees;
        }

        return 0.0f;
    }

    void ecs_utils::set_linear_velocity(entt::registry& registry, entt::entity entity,
                                        const glm::vec2& velocity) {
        if (auto* vel = registry.try_get<component_velocity>(entity)) {
            vel->linear = velocity;
        }
    }

    void ecs_utils::add_impulse(entt::registry& registry, entt::entity entity,
                                const glm::vec2& impulse) {
        if (auto* vel = registry.try_get<component_velocity>(entity)) {
            vel->linear += impulse;
        }
    }

}  // namespace engine
