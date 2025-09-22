#include "entities.hxx"

#include "../engine.hxx"

namespace engine {
    void game_entities::system_physics_update(const float tick_interval) {
        system_physics::update(m_registry, tick_interval);
    }

    void game_entities::system_lifetime_update(const float tick_interval) {
        system_lifetime::update(m_registry, tick_interval);
    }

    void game_entities::system_renderer_update(game_renderer* renderer, game_resources& resources,
                                               const float fraction_to_next_tick) {
        system_renderer::update(m_registry, renderer, resources, fraction_to_next_tick);
    }

    entt::entity game_entities::sprite_create(std::string_view resource_key) {
        entt::entity entity = m_registry.create();

        m_registry.emplace<component_transform>(entity, glm::vec2{0.0f, 0.0f}, 0.0f,
                                                glm::vec2{1.0f, 1.0f});
        m_registry.emplace<component_sprite>(entity, resource_key);
        m_registry.emplace<component_renderable>(entity, true, 0);

        return entity;
    }

    entt::entity game_entities::sprite_create_interpolated(std::string_view resource_key) {
        entt::entity entity = sprite_create(resource_key);

        m_registry.emplace<component_velocity_linear>(entity, glm::vec2{0.0f, 0.0f}, 0.0f, 0.0f);
        m_registry.emplace<component_velocity_angular>(entity, 0.0f, 0.0f, 0.0f);
        m_registry.emplace<component_interpolation>(entity, glm::vec2{0.0f, 0.0f}, 0.0f);

        return entity;
    }

    entt::entity game_entities::create_text_dynamic(std::string_view resource_key) {
        entt::entity entity = m_registry.create();

        m_registry.emplace<component_transform>(entity, glm::vec2{0.0f, 0.0f}, 0.0f,
                                                glm::vec2{1.0f, 1.0f});
        m_registry.emplace<component_text_dynamic>(entity, resource_key);
        m_registry.emplace<component_renderable>(entity, true, 0);

        return entity;
    }

    void game_entities::set_transform_position(entt::entity entity, const glm::vec2& position) {
        if (auto* transform = m_registry.try_get<component_transform>(entity); transform) {
            transform->position = position;
        }
    }

    glm::vec2 game_entities::get_transform_position(entt::entity entity) const {
        if (const auto* transform = m_registry.try_get<component_transform>(entity); transform) {
            return transform->position;
        }

        return glm::vec2{0.0f, 0.0f};
    }

    glm::vec2 game_entities::get_interpolated_position(entt::entity entity,
                                                       float fraction_to_next_tick) const {
        if (const auto* transform = m_registry.try_get<component_transform>(entity); transform) {
            if (const auto* interp = m_registry.try_get<component_interpolation>(entity); interp) {
                return glm::mix(interp->previous_position, transform->position,
                                fraction_to_next_tick);
            }

            return transform->position;
        }

        return glm::vec2{0.0f, 0.0f};
    }

    float game_entities::get_interpolated_rotation(entt::entity entity,
                                                   float fraction_to_next_tick) const {
        if (const auto* transform = m_registry.try_get<component_transform>(entity)) {
            if (const auto* interp = m_registry.try_get<component_interpolation>(entity)) {
                return glm::mix(interp->previous_rotation, transform->rotation,
                                fraction_to_next_tick);
            }

            return transform->rotation;
        }

        return 0.f;
    }

    void game_entities::set_transform_scale(entt::entity entity, const glm::vec2& new_scale) {
        if (auto* transform = m_registry.try_get<component_transform>(entity); transform) {
            transform->scale = new_scale;
        }
    }

    glm::vec2 game_entities::get_transform_scale(entt::entity entity) {
        if (const auto* transform = m_registry.try_get<component_transform>(entity); transform) {
            return transform->scale;
        }

        return glm::vec2{1.0f, 1.0f};
    }

    glm::vec2 game_entities::get_vector_forward(entt::entity entity) const {
        if (const auto* transform = m_registry.try_get<component_transform>(entity); transform) {
            float radians = glm::radians(transform->rotation + 90.f);
            return glm::vec2{glm::cos(radians), glm::sin(radians)};
        }

        return glm::vec2{0.0f, 1.0f};
    }

    glm::vec2 game_entities::get_vector_right(entt::entity entity) const {
        if (const auto* transform = m_registry.try_get<component_transform>(entity); transform) {
            float radians = glm::radians(transform->rotation);
            return glm::vec2{glm::cos(radians), glm::sin(radians)};
        }

        return glm::vec2{1.0f, 0.0f};
    }

    void game_entities::add_impulse_forward(entt::entity entity, float magnitude) {
        auto forward = get_vector_forward(entity);
        add_impulse_velocity_linear(entity, forward * magnitude);
    }

    void game_entities::add_impulse_backward(entt::entity entity, float magnitude) {
        add_impulse_forward(entity, -magnitude);
    }

    void game_entities::add_impulse_right(entt::entity entity, float magnitude) {
        auto right = get_vector_right(entity);
        add_impulse_velocity_linear(entity, right * magnitude);
    }

    void game_entities::add_impulse_left(entt::entity entity, float magnitude) {
        add_impulse_right(entity, -magnitude);
    }

    void game_entities::add_impulse_direction(entt::entity entity, float angle_degrees,
                                              float magnitude) {
        // Convert angle to radians and create a direction vector
        const float radians = glm::radians(angle_degrees);
        const glm::vec2 direction = {std::cos(radians), std::sin(radians)};
        add_impulse_velocity_linear(entity, direction * magnitude);
    }

    void game_entities::add_impulse_relative(entt::entity entity,
                                             const glm::vec2& relative_direction, float magnitude) {
        if (const auto* transform = m_registry.try_get<component_transform>(entity); transform) {
            // Rotate the relative direction by the entity's rotation
            const float radians = glm::radians(transform->rotation);
            glm::vec2 rotated_direction = {
                relative_direction.x * glm::cos(radians) - relative_direction.y * glm::sin(radians),
                relative_direction.x * glm::sin(radians) +
                    relative_direction.y * glm::cos(radians)};
            rotated_direction = glm::normalize(rotated_direction);
            add_impulse_velocity_linear(entity, rotated_direction * magnitude);
        }
    }

    void game_entities::set_velocity_linear(entt::entity entity, const glm::vec2& velocity) {
        if (auto* vel = m_registry.try_get<component_velocity_linear>(entity); vel) {
            vel->value = velocity;
        }
    }

    void game_entities::add_impulse_velocity_linear(entt::entity entity, const glm::vec2& impulse) {
        if (auto* vel = m_registry.try_get<component_velocity_linear>(entity); vel) {
            vel->value += impulse;
        }
    }

    void game_entities::set_velocity_linear_drag(entt::entity entity, float linear_drag) {
        if (auto* vel = m_registry.try_get<component_velocity_linear>(entity); vel) {
            vel->drag = linear_drag;
        }
    }

    void game_entities::set_velocity_linear_max(entt::entity entity, float max_speed) {
        if (auto* vel = m_registry.try_get<component_velocity_linear>(entity); vel) {
            vel->max_speed = max_speed;
        }
    }

    void game_entities::set_velocity_angular(entt::entity entity, float angular_velocity) {
        if (auto* vel = m_registry.try_get<component_velocity_angular>(entity); vel) {
            vel->value = angular_velocity;
        }
    }

    void game_entities::add_impulse_velocity_angular(entt::entity entity, float angular_impulse) {
        if (auto* vel = m_registry.try_get<component_velocity_angular>(entity); vel) {
            vel->value += angular_impulse;
        }
    }

    void game_entities::set_velocity_angular_drag(entt::entity entity, float angular_drag) {
        if (auto* vel = m_registry.try_get<component_velocity_angular>(entity); vel) {
            vel->drag = angular_drag;
        }
    }

    void game_entities::set_velocity_angular_max(entt::entity entity, float max_angular_speed) {
        if (auto* vel = m_registry.try_get<component_velocity_angular>(entity); vel) {
            vel->max_speed = max_angular_speed;
        }
    }

    void game_entities::set_renderable_visible(entt::entity entity, bool is_visible) {
        if (auto* renderable = m_registry.try_get<component_renderable>(entity); renderable) {
            renderable->is_visible = is_visible;
        }
    }

    void game_entities::set_renderable_layer(entt::entity entity, int layer) {
        if (auto* renderable = m_registry.try_get<component_renderable>(entity); renderable) {
            renderable->layer = layer;
        }
    }
}  // namespace engine
