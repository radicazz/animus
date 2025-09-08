#include "ecs_manager.hxx"
#include "../engine.hxx"

namespace engine {

    ecs_manager::ecs_manager() = default;

    ecs_manager::~ecs_manager() = default;

    void ecs_manager::update_physics(float delta_time) {
        system_physics::update(m_registry, delta_time);
    }

    void ecs_manager::update_lifetime(float delta_time) {
        system_lifetime::update(m_registry, delta_time);
    }

    void ecs_manager::render_sprites(game_renderer& renderer, float interpolation_alpha) {
        system_renderer::render(m_registry, renderer, interpolation_alpha);
    }

    entt::entity ecs_manager::create_sprite_entity(const glm::vec2& position,
                                                   std::unique_ptr<render_sprite> sprite) {
        return ecs_utils::create_sprite_entity(m_registry, position, std::move(sprite));
    }

    void ecs_manager::destroy_entity(entt::entity entity) {
        if (m_registry.valid(entity)) {
            m_registry.destroy(entity);
        }
    }

    bool ecs_manager::is_valid(entt::entity entity) const {
        return m_registry.valid(entity);
    }

    void ecs_manager::set_position(entt::entity entity, const glm::vec2& position) {
        ecs_utils::set_position(m_registry, entity, position);
    }

    glm::vec2 ecs_manager::get_position(entt::entity entity) const {
        return ecs_utils::get_position(const_cast<entt::registry&>(m_registry), entity);
    }

    void ecs_manager::set_velocity(entt::entity entity, const glm::vec2& velocity) {
        ecs_utils::set_linear_velocity(m_registry, entity, velocity);
    }

    void ecs_manager::add_impulse(entt::entity entity, const glm::vec2& impulse) {
        ecs_utils::add_impulse(m_registry, entity, impulse);
    }

    void ecs_manager::clear_all_entities() {
        m_registry.clear();
    }

}  // namespace engine
