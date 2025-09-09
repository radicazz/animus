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
        void update_physics(float fixed_delta_time);
        void update_lifetime(float fixed_delta_time);
        void render_sprites(game_renderer& renderer, float interpolation_alpha);

        /**
         * @brief Create a sprite entity with interpolated transform and sprite components.
         * @param position Initial world position of the entity.
         * @param sprite Unique pointer to the sprite to assign to the entity.
         * @param layer Rendering layer for the sprite (default is 0).
         * @return The created entity.
         */
        entt::entity create_interpolated_sprite(const glm::vec2& position,
                                                std::unique_ptr<render_sprite> sprite,
                                                int layer = 0);

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
        glm::vec2 get_interpolated_position(entt::entity entity, float alpha) const;

        void set_linear_velocity(entt::entity entity, const glm::vec2& velocity);
        void set_angular_velocity(entt::entity entity, float angular);

        void add_linear_impulse(entt::entity entity, const glm::vec2& impulse);
        void add_angular_impulse(entt::entity entity, float impulse);

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

    inline void game_entities::destroy(entt::entity entity) {
        if (m_registry.valid(entity)) {
            m_registry.destroy(entity);
        }
    }

    inline bool game_entities::valid(entt::entity entity) const {
        return m_registry.valid(entity);
    }

    inline void game_entities::clear() {
        m_registry.clear();
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
