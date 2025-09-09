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

    void game_entities::render(game_renderer& renderer, float interpolation_alpha) {
        system_renderer::render(m_registry, renderer, interpolation_alpha);
    }

    entt::entity game_entities::create_sprite(const glm::vec2& position,
                                              std::unique_ptr<render_sprite> sprite) {
        return ecs_utils::create_sprite_entity(m_registry, position, std::move(sprite));
    }

    void game_entities::destroy(entt::entity entity) {
        if (m_registry.valid(entity)) {
            m_registry.destroy(entity);
        }
    }

    bool game_entities::valid(entt::entity entity) const {
        return m_registry.valid(entity);
    }

    void game_entities::clear() {
        m_registry.clear();
    }

    void game_entities::set_position(entt::entity entity, const glm::vec2& position) {
        ecs_utils::set_position(m_registry, entity, position);
    }

    glm::vec2 game_entities::get_position(entt::entity entity) const {
        return ecs_utils::get_position(const_cast<entt::registry&>(m_registry), entity);
    }

    void game_entities::set_velocity(entt::entity entity, const glm::vec2& velocity) {
        ecs_utils::set_linear_velocity(m_registry, entity, velocity);
    }

    void game_entities::add_impulse(entt::entity entity, const glm::vec2& impulse) {
        ecs_utils::add_impulse(m_registry, entity, impulse);
    }

}  // namespace engine
