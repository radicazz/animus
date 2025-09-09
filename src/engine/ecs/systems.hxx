#pragma once

#include <entt/entt.hpp>
#include "components.hxx"

namespace engine {
    class game_renderer;

    /**
     * @brief Physics system that handles velocity integration and movement
     */
    class system_physics {
    public:
        static void tick(entt::registry& registry, float delta_time);
    };

    /**
     * @brief Rendering system for sprites with ECS components
     * @note currently only renders entities with sprites and with interpolated transform components
     */
    class system_renderer {
    public:
        static void tick(entt::registry& registry, game_renderer& renderer,
                         float interpolation_alpha);
    };

    /**
     * @brief Lifetime system that handles entity expiration
     */
    class system_lifetime {
    public:
        static void tick(entt::registry& registry, float delta_time);
    };
}  // namespace engine
