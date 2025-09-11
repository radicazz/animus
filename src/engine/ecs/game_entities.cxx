#include "game_entities.hxx"
#include "../engine.hxx"

namespace engine {
    void game_entities::system_physics_update(const float tick_interval) {
        system_physics::update(m_registry, tick_interval);
    }

    void game_entities::system_lifetime_update(const float tick_interval) {
        system_lifetime::update(m_registry, tick_interval);
    }

    void game_entities::system_renderer_update(game_renderer& renderer, game_resources& resources,
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

    void game_entities::set_velocity_linear(entt::entity entity, const glm::vec2& velocity) {
        if (auto* vel = m_registry.try_get<component_velocity_linear>(entity); vel) {
            vel->value = velocity;
        }
    }

    void game_entities::add_impulse_linear(entt::entity entity, const glm::vec2& impulse) {
        if (auto* vel = m_registry.try_get<component_velocity_linear>(entity); vel) {
            vel->value += impulse;
        }
    }

    void game_entities::set_velocity_angular(entt::entity entity, float angular_velocity) {
        if (auto* vel = m_registry.try_get<component_velocity_angular>(entity); vel) {
            vel->value = angular_velocity;
        }
    }

    void game_entities::add_impulse_angular(entt::entity entity, float angular_impulse) {
        if (auto* vel = m_registry.try_get<component_velocity_angular>(entity); vel) {
            vel->value += angular_impulse;
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
