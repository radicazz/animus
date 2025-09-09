#include "game_entities.hxx"
#include "../engine.hxx"

namespace engine {

    game_entities::game_entities() = default;

    game_entities::~game_entities() = default;

    void game_entities::update_physics(float delta_time) {
        system_physics::update(m_registry, delta_time);
    }

    void game_entities::update_lifetime(float delta_time) {
        system_lifetime::update(m_registry, delta_time);
    }

    void game_entities::render_sprites(game_renderer& renderer, float interpolation_alpha) {
        system_renderer::render(m_registry, renderer, interpolation_alpha);
    }

    entt::entity game_entities::create_interpolated_sprite(const glm::vec2& position,
                                                           std::unique_ptr<render_sprite> sprite,
                                                           int layer) {
        entt::entity entity = m_registry.create();

        // Add transform component
        auto& transform_comp = m_registry.emplace<component_transform_interpolated>(entity);
        transform_comp.position = position;
        transform_comp.previous_position = position;

        // Add sprite component
        auto& sprite_comp = m_registry.emplace<component_sprite>(entity);
        sprite_comp.sprite = std::move(sprite);
        sprite_comp.layer = layer;

        return entity;
    }

    void game_entities::set_position(entt::entity entity, const glm::vec2& position) {
        auto* transform = m_registry.try_get<component_transform_interpolated>(entity);
        if (transform) {
            transform->position = position;
        }
    }

    glm::vec2 game_entities::get_position(entt::entity entity) const {
        auto* transform = m_registry.try_get<component_transform_interpolated>(entity);
        return transform ? transform->position : glm::vec2{0.0f, 0.0f};
    }

    glm::vec2 game_entities::get_interpolated_position(entt::entity entity, float alpha) const {
        auto* transform = m_registry.try_get<component_transform_interpolated>(entity);
        if (!transform) {
            return glm::vec2{0.0f, 0.0f};
        }
        return glm::mix(transform->previous_position, transform->position, alpha);
    }

    void game_entities::set_linear_velocity(entt::entity entity, const glm::vec2& velocity) {
        auto* vel = m_registry.try_get<component_velocity>(entity);
        if (vel) {
            vel->linear = velocity;
        }
    }

    void game_entities::set_angular_velocity(entt::entity entity, float angular) {
        auto* vel = m_registry.try_get<component_velocity>(entity);
        if (vel) {
            vel->angular = angular;
        }
    }

    void game_entities::add_linear_impulse(entt::entity entity, const glm::vec2& impulse) {
        auto* vel = m_registry.try_get<component_velocity>(entity);
        if (vel) {
            vel->linear += impulse;
        }
    }

    void game_entities::add_angular_impulse(entt::entity entity, float impulse) {
        auto* vel = m_registry.try_get<component_velocity>(entity);
        if (vel) {
            vel->angular += impulse;
        }
    }

}  // namespace engine
