#include "viewport.hxx"
#include "camera.hxx"

#include <SDL3/SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace engine {
    void game_viewport::apply_to_sdl(SDL_Renderer* sdl_renderer) const {
        if (sdl_renderer == nullptr) {
            return;
        }

        SDL_Rect rect{m_position_pixels.x, m_position_pixels.y, m_size_pixels.x, m_size_pixels.y};
        SDL_SetRenderViewport(sdl_renderer, &rect);
    }

    glm::mat3 game_viewport::get_view_matrix(const game_camera& camera) const {
        // Transform world -> screen inside this viewport:
        // screen = (world - camera_pos) * zoom + viewport_top_left + viewport_size/2
        glm::mat3 view(1.0f);

        const float zoom = camera.get_zoom();

        // Scale
        view[0][0] = zoom;
        view[1][1] = zoom;

        // Translation
        // Center camera in the viewport: origin at viewport center
        const glm::vec2 screen_center = m_position_pixels + m_size_pixels * 0.5f;
        view[2][0] = -camera.get_position().x * zoom + screen_center.x;
        view[2][1] = -camera.get_position().y * zoom + screen_center.y;

        return view;
    }

    glm::vec2 game_viewport::world_to_screen(const game_camera& camera,
                                             const glm::vec2& world_pos) const {
        glm::vec3 world_h = glm::vec3(world_pos, 1.0f);
        glm::vec3 screen_h = get_view_matrix(camera) * world_h;
        return {screen_h.x, screen_h.y};
    }

    glm::vec2 game_viewport::screen_to_world(const game_camera& camera,
                                             const glm::vec2& screen_pos) const {
        const glm::vec2 screen_center = m_position_pixels + m_size_pixels * 0.5f;
        const glm::vec2 centered = screen_pos - screen_center;
        return camera.get_position() + centered / camera.get_zoom();
    }

    std::tuple<glm::vec2, glm::vec2> game_viewport::get_visible_area_world(
        const game_camera& camera) const {
        const glm::vec2 half_viewport_world = (m_size_pixels * 0.5f) / camera.get_zoom();
        const glm::vec2 cam = camera.get_position();

        const float min_x = cam.x - half_viewport_world.x;
        const float min_y = cam.y - half_viewport_world.y;
        const float max_x = cam.x + half_viewport_world.x;
        const float max_y = cam.y + half_viewport_world.y;

        return {glm::vec2(min_x, min_y), glm::vec2(max_x, max_y)};
    }

    bool game_viewport::is_in_view(const game_camera& camera, const glm::vec2& position,
                                   const glm::vec2& size) const {
        const auto [min_bounds, max_bounds] = get_visible_area_world(camera);

        const float obj_left = position.x - size.x * 0.5f;
        const float obj_right = position.x + size.x * 0.5f;
        const float obj_top = position.y - size.y * 0.5f;
        const float obj_bottom = position.y + size.y * 0.5f;

        return !(obj_right < min_bounds.x || obj_left > max_bounds.x || obj_bottom < min_bounds.y ||
                 obj_top > max_bounds.y);
    }

    void game_viewport::clamp_camera_to_bounds(game_camera& camera) const {
        // If the camera has bounds, clamp against half visible size in world units
        const glm::vec2 half_viewport_world = (m_size_pixels * 0.5f) / camera.get_zoom();
        camera.clamp_to_physical_bounds(half_viewport_world);
    }

}  // namespace engine
