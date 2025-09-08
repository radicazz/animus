#pragma once

#include <entt/entt.hpp>
#include "systems.hxx"
#include "components.hxx"

namespace engine {
    class game_engine;

    /**
     * @brief ECS Manager that coordinates all systems and provides easy access to the registry
     */
    class ecs_manager {
    public:
        ecs_manager();
        ~ecs_manager();

        ecs_manager(const ecs_manager&) = delete;
        ecs_manager& operator=(const ecs_manager&) = delete;
        ecs_manager(ecs_manager&&) = delete;
        ecs_manager& operator=(ecs_manager&&) = delete;

        [[nodiscard]] entt::registry& get_registry();
        [[nodiscard]] const entt::registry& get_registry() const;

        void update_physics(float fixed_delta_time);
        void update_lifetime(float fixed_delta_time);
        void render_sprites(game_renderer& renderer, float interpolation_alpha);

        entt::entity create_sprite_entity(const glm::vec2& position,
                                          std::unique_ptr<render_sprite> sprite);
        void destroy_entity(entt::entity entity);
        bool is_valid(entt::entity entity) const;

        // Component access templates
        template <typename Component>
        Component& get_component(entt::entity entity);

        template <typename Component>
        const Component& get_component(entt::entity entity) const;

        template <typename Component>
        Component* try_get_component(entt::entity entity);

        template <typename Component>
        const Component* try_get_component(entt::entity entity) const;

        template <typename Component, typename... Args>
        Component& add_component(entt::entity entity, Args&&... args);

        template <typename Component>
        void remove_component(entt::entity entity);

        template <typename... Components>
        bool has_components(entt::entity entity) const;

        // Query methods
        template <typename... Components>
        auto get_entities_with();

        // Utility methods
        void set_position(entt::entity entity, const glm::vec2& position);
        glm::vec2 get_position(entt::entity entity) const;

        void set_velocity(entt::entity entity, const glm::vec2& velocity);
        void add_impulse(entt::entity entity, const glm::vec2& impulse);

        // Clear all entities (useful for scene transitions)
        void clear_all_entities();

    private:
        entt::registry m_registry;
    };

    // Inline implementations
    inline entt::registry& ecs_manager::get_registry() {
        return m_registry;
    }

    inline const entt::registry& ecs_manager::get_registry() const {
        return m_registry;
    }

    template <typename Component>
    inline Component& ecs_manager::get_component(entt::entity entity) {
        return m_registry.get<Component>(entity);
    }

    template <typename Component>
    inline const Component& ecs_manager::get_component(entt::entity entity) const {
        return m_registry.get<Component>(entity);
    }

    template <typename Component>
    inline Component* ecs_manager::try_get_component(entt::entity entity) {
        return m_registry.try_get<Component>(entity);
    }

    template <typename Component>
    inline const Component* ecs_manager::try_get_component(entt::entity entity) const {
        return m_registry.try_get<Component>(entity);
    }

    template <typename Component, typename... Args>
    inline Component& ecs_manager::add_component(entt::entity entity, Args&&... args) {
        return m_registry.emplace<Component>(entity, std::forward<Args>(args)...);
    }

    template <typename Component>
    inline void ecs_manager::remove_component(entt::entity entity) {
        m_registry.remove<Component>(entity);
    }

    template <typename... Components>
    inline bool ecs_manager::has_components(entt::entity entity) const {
        return m_registry.all_of<Components...>(entity);
    }

    template <typename... Components>
    inline auto ecs_manager::get_entities_with() {
        return m_registry.view<Components...>();
    }

}  // namespace engine
