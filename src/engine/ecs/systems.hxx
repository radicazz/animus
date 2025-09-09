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
        static void update(entt::registry& registry, float fixed_delta_time);

    private:
        static void integrate_velocity(entt::registry& registry, float fixed_delta_time);
    };

    /**
     * @brief Rendering system for sprites with ECS components
     * @note currently only renders entities with sprites and with interpolated transform components
     */
    class system_renderer {
    public:
        static void render(entt::registry& registry, game_renderer& renderer,
                           float interpolation_alpha);

    private:
        static void render_sprites_interpolated(entt::registry& registry, game_renderer& renderer,
                                                float interpolation_alpha);
    };

    /**
     * @brief Lifetime system that handles entity expiration
     */
    class system_lifetime {
    public:
        static void update(entt::registry& registry, float fixed_delta_time);
    };
}  // namespace engine
