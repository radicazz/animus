#pragma once

#include <entt/entt.hpp>
#include "components.hxx"

namespace engine {
    class game_renderer;
    class game_resources;

    class system_physics {
    public:
        static void update(entt::registry& registry, float tick_interval);

    private:
        static void integrate_velocity(entt::registry& registry, float tick_interval);
    };

    /**
     * @brief Rendering system for sprites with ECS components
     * @note currently only renders entities with sprites and with interpolated transform components
     */
    class system_renderer {
    public:
        static void update(entt::registry& registry, game_renderer* renderer,
                           game_resources& resources, float fraction_to_next_tick);
    };

    /**
     * @brief Lifetime system that handles entity expiration
     */
    class system_lifetime {
    public:
        static void update(entt::registry& registry, float tick_interval);
    };
}  // namespace engine
