#pragma once

#include <entt/entt.hpp>
#include "components.hxx"

namespace engine {
    class game_engine;

    /**
     * @brief Physics system that handles velocity integration and movement
     */
    class system_physics {
    public:
        static void update(entt::registry& registry, float delta_time);

    private:
        static void apply_drag(entt::registry& registry, float delta_time);
        static void enforce_speed_limits(entt::registry& registry);
        static void integrate_velocity(entt::registry& registry, float delta_time);
    };

    /**
     * @brief Rendering system for sprites with ECS components
     */
    class system_renderer {
    public:
        static void render(entt::registry& registry, game_engine* engine);
    };

    /**
     * @brief Lifetime system that handles entity expiration
     */
    class system_lifetime {
    public:
        static void update(entt::registry& registry, float delta_time);
    };

    /**
     * @brief Utility functions for ECS operations
     */
    class ecs_utils {
    public:
        // Entity creation helpers
        static entt::entity create_sprite_entity(entt::registry& registry,
                                                 const glm::vec2& position,
                                                 std::unique_ptr<render_sprite> sprite);

        // Component helpers
        static void set_position(entt::registry& registry, entt::entity entity,
                                 const glm::vec2& position);
        static glm::vec2 get_position(entt::registry& registry, entt::entity entity);
        static void set_velocity(entt::registry& registry, entt::entity entity,
                                 const glm::vec2& velocity);
        static void add_impulse(entt::registry& registry, entt::entity entity,
                                const glm::vec2& impulse);

        // Query helpers
        template <typename... Components>
        static auto get_entities_with(entt::registry& registry) {
            return registry.view<Components...>();
        }
    };

}  // namespace engine