#include "game_entities.hxx"
#include "../engine.hxx"

namespace engine {
    void game_entities::tick_physics(float delta_time) {
        system_physics::tick(m_registry, delta_time);
    }

    void game_entities::tick_lifetime(float delta_time) {
        system_lifetime::tick(m_registry, delta_time);
    }

    void game_entities::tick_renderer(game_renderer& renderer, float alpha) {
        system_renderer::tick(m_registry, renderer, alpha);
    }

    entt::entity game_entities::create_sprite_static(const glm::vec2& position,
                                                     game_sprite::uptr sprite, int layer) {
        entt::entity entity = m_registry.create();

        m_registry.emplace<component_transform>(entity, position, 0.0f, glm::vec2{1.0f, 1.0f});
        m_registry.emplace<component_sprite_single>(entity, std::move(sprite));
        m_registry.emplace<component_renderable>(entity, true, layer);

        return entity;
    }

    entt::entity game_entities::create_sprite_interpolated(const glm::vec2& position,
                                                           game_sprite::uptr sprite, int layer) {
        entt::entity entity = create_sprite_static(position, std::move(sprite), layer);

        m_registry.emplace<component_velocity_linear>(entity, glm::vec2{0.0f, 0.0f}, 0.0f, 0.0f);
        m_registry.emplace<component_velocity_angular>(entity, 0.0f, 0.0f, 0.0f);
        m_registry.emplace<component_interpolation>(entity, position, 0.0f);

        return entity;
    }

    void game_entities::set_position(entt::entity entity, const glm::vec2& position) {
        if (auto* transform = m_registry.try_get<component_transform>(entity); transform) {
            transform->position = position;
        }
    }

    glm::vec2 game_entities::get_position(entt::entity entity) const {
        if (const auto* transform = m_registry.try_get<component_transform>(entity); transform) {
            return transform->position;
        }

        return glm::vec2{0.0f, 0.0f};
    }

    glm::vec2 game_entities::get_interpolated_position(entt::entity entity, float alpha) const {
        if (const auto* transform = m_registry.try_get<component_transform>(entity); transform) {
            if (const auto* interp = m_registry.try_get<component_interpolation>(entity); interp) {
                return glm::mix(interp->previous_position, transform->position, alpha);
            }

            return transform->position;
        }

        return glm::vec2{0.0f, 0.0f};
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

    void game_entities::set_visible(entt::entity entity, bool is_visible) {
        if (auto* renderable = m_registry.try_get<component_renderable>(entity); renderable) {
            renderable->is_visible = is_visible;
        }
    }

    void game_entities::set_layer(entt::entity entity, int layer) {
        if (auto* renderable = m_registry.try_get<component_renderable>(entity); renderable) {
            renderable->layer = layer;
        }
    }
}  // namespace engine
