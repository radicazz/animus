#pragma once

#include <entt/entt.hpp>
#include "systems.hxx"
#include "components.hxx"

namespace engine {
    class game_engine;
    class game_renderer;

    /**
     * @brief Game entities manager that coordinates all systems and provides easy access to the
     * registry
     */
    class game_entities {
    public:
        game_entities();
        ~game_entities();

        game_entities(const game_entities&) = delete;
        game_entities& operator=(const game_entities&) = delete;
        game_entities(game_entities&&) = delete;
        game_entities& operator=(game_entities&&) = delete;

        // Registry access
        [[nodiscard]] entt::registry& registry();
        [[nodiscard]] const entt::registry& registry() const;

        // System updates
        void update_physics(float delta_time);
        void update_lifetime(float delta_time);
        void render(game_renderer& renderer, float interpolation_alpha);

        // Entity management
        entt::entity create_sprite(const glm::vec2& position,
                                   std::unique_ptr<render_sprite> sprite);
        void destroy(entt::entity entity);
        bool valid(entt::entity entity) const;
        void clear();

        // Component access - simplified API
        template <typename Component>
        Component& get(entt::entity entity);

        template <typename Component>
        const Component& get(entt::entity entity) const;

        template <typename Component>
        Component* try_get(entt::entity entity);

        template <typename Component>
        const Component* try_get(entt::entity entity) const;

        template <typename Component, typename... Args>
        Component& add(entt::entity entity, Args&&... args);

        template <typename Component>
        void remove(entt::entity entity);

        template <typename... Components>
        bool has(entt::entity entity) const;

        // Query methods
        template <typename... Components>
        auto view();

        // Utility methods for common operations
        void set_position(entt::entity entity, const glm::vec2& position);
        glm::vec2 get_position(entt::entity entity) const;

        void set_velocity(entt::entity entity, const glm::vec2& velocity);
        void add_impulse(entt::entity entity, const glm::vec2& impulse);

    private:
        entt::registry m_registry;
    };

    // Inline implementations
    inline entt::registry& game_entities::registry() {
        return m_registry;
    }

    inline const entt::registry& game_entities::registry() const {
        return m_registry;
    }

    template <typename Component>
    inline Component& game_entities::get(entt::entity entity) {
        return m_registry.get<Component>(entity);
    }

    template <typename Component>
    inline const Component& game_entities::get(entt::entity entity) const {
        return m_registry.get<Component>(entity);
    }

    template <typename Component>
    inline Component* game_entities::try_get(entt::entity entity) {
        return m_registry.try_get<Component>(entity);
    }

    template <typename Component>
    inline const Component* game_entities::try_get(entt::entity entity) const {
        return m_registry.try_get<Component>(entity);
    }

    template <typename Component, typename... Args>
    inline Component& game_entities::add(entt::entity entity, Args&&... args) {
        return m_registry.emplace<Component>(entity, std::forward<Args>(args)...);
    }

    template <typename Component>
    inline void game_entities::remove(entt::entity entity) {
        m_registry.remove<Component>(entity);
    }

    template <typename... Components>
    inline bool game_entities::has(entt::entity entity) const {
        return m_registry.all_of<Components...>(entity);
    }

    template <typename... Components>
    inline auto game_entities::view() {
        return m_registry.view<Components...>();
    }

}  // namespace engine
