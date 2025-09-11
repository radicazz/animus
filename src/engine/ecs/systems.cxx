#include "systems.hxx"

#include <glm/glm.hpp>
#include "../engine.hxx"
#include "../utils/resource_manager.hxx"

#include <algorithm>
#include <vector>
#include <cmath>

namespace engine {
    void system_physics::update(entt::registry& registry, const float tick_interval) {
        integrate_velocity(registry, tick_interval);
    }

    void system_physics::integrate_velocity(entt::registry& registry, float tick_interval) {
        auto view = registry.view<component_transform>();

        for (auto [entity, transform] : view.each()) {
            // If interpolation component exists, store previous position and rotation for later.
            if (auto* interpolation = registry.try_get<component_interpolation>(entity)) {
                interpolation->previous_position = transform.position;
                interpolation->previous_rotation = transform.rotation;
            }

            if (auto* velocity_linear = registry.try_get<component_velocity_linear>(entity)) {
                glm::vec2 velocity = velocity_linear->value;

                if (velocity_linear->drag > 0.0f) {
                    const float drag_factor = 1.0f - (velocity_linear->drag * tick_interval);
                    velocity *= std::max(0.0f, drag_factor);
                    velocity_linear->value = velocity;
                }

                if (velocity_linear->max_speed > 0.0f) {
                    const float speed = glm::length(velocity);
                    if (speed > velocity_linear->max_speed) {
                        velocity = (velocity / speed) * velocity_linear->max_speed;
                        velocity_linear->value = velocity;
                    }
                }

                transform.position += velocity * tick_interval;
            }

            if (auto* angular_velocity = registry.try_get<component_velocity_angular>(entity)) {
                float velocity = angular_velocity->value;

                if (angular_velocity->drag > 0.0f) {
                    const float drag_factor = 1.0f - (angular_velocity->drag * tick_interval);
                    velocity *= std::max(0.0f, drag_factor);
                    angular_velocity->value = velocity;
                }

                if (angular_velocity->max_speed > 0.0f) {
                    if (std::abs(velocity) > angular_velocity->max_speed) {
                        velocity = std::copysign(angular_velocity->max_speed, velocity);
                        angular_velocity->value = velocity;
                    }
                }

                transform.rotation += velocity * tick_interval;

                // Normalize rotation.
                while (transform.rotation >= 360.0f) {
                    transform.rotation -= 360.0f;
                }

                while (transform.rotation < 0.0f) {
                    transform.rotation += 360.0f;
                }
            }
        }
    }

    void system_renderer::update(entt::registry& registry, game_renderer& renderer,
                                 game_resources& resources, const float fraction_to_next_tick) {
        // TODO: Optimize all of this rubbish. Also, make the layering work.

        // Render sprites.
        auto resource_sprite_view =
            registry.view<component_transform, component_renderable, component_sprite>();
        for (auto [entity, transform, renderable, sprite_comp] : resource_sprite_view.each()) {
            if (renderable.is_visible == false) {
                continue;
            }

            if (auto* sprite = resources.sprite_get(sprite_comp.resource_key)) {
                glm::vec2 render_position = transform.position;
                float render_rotation = transform.rotation;

                // Apply interpolation if available
                if (auto* interp = registry.try_get<component_interpolation>(entity)) {
                    render_position = glm::mix(interp->previous_position, transform.position,
                                               fraction_to_next_tick);

                    // Interpolate rotation with wrap-around
                    const float previous_rotation = interp->previous_rotation;
                    const float current_rotation = transform.rotation;

                    float rotation_diff = current_rotation - previous_rotation;
                    if (rotation_diff > 180.0f) {
                        rotation_diff -= 360.0f;
                    } else if (rotation_diff < -180.0f) {
                        rotation_diff += 360.0f;
                    }

                    render_rotation = previous_rotation + (rotation_diff * fraction_to_next_tick);
                }

                sprite->set_rotation(render_rotation);
                sprite->set_scale(transform.scale);

                renderer.sprite_draw_world(sprite, render_position);
            }
        }

        // Render dynamic text
        auto dynamic_text_view =
            registry.view<component_transform, component_renderable, component_text_dynamic>();
        for (auto [entity, transform, renderable, text_comp] : dynamic_text_view.each()) {
            if (renderable.is_visible == false) {
                continue;
            }

            if (auto* text = resources.text_dynamic_get(text_comp.resource_key)) {
                glm::vec2 render_position = transform.position;

                // Apply interpolation if available
                if (auto* interp = registry.try_get<component_interpolation>(entity)) {
                    render_position = glm::mix(interp->previous_position, transform.position,
                                               fraction_to_next_tick);
                }

                // Apply transform properties
                text->set_scale(transform.scale);
                text->set_rotation(transform.rotation);

                renderer.text_draw_world(text, render_position);
            }
        }
    }

    // Lifetime System Implementation
    void system_lifetime::update(entt::registry& registry, float tick_interval) {
        auto view = registry.view<component_lifetime>();
        std::vector<entt::entity> entities_to_destroy;

        for (auto [entity, lifetime] : view.each()) {
            lifetime.remaining_seconds -= tick_interval;

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
