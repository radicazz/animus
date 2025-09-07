#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <unordered_set>

namespace engine {
    enum class input_key {
        none,
        w,
        a,
        s,
        d,
        c,
        o,
        p,
        arrow_up,
        arrow_down,
        arrow_left,
        arrow_right,
        space,
        escape,
        enter,
        mouse_left,
        mouse_right,
        mouse_middle
    };

    enum class input_state { pressed, held, released };

    class input_system {
    public:
        input_system();

        void update();
        void process_event(const SDL_Event& event);

        [[nodiscard]] bool is_key_pressed(input_key key) const;
        [[nodiscard]] bool is_key_held(input_key key) const;
        [[nodiscard]] bool is_key_released(input_key key) const;

        /**
         * @brief Get the current mouse position in screen space.
         * @return The mouse position as a glm::vec2.
         */
        [[nodiscard]] glm::vec2 get_mouse_screen_position() const;

        /**
         * @brief Get the mouse movement delta since the last frame.
         *
         * This function returns the change in mouse position (delta) since the last frame,
         * allowing you to track how much the mouse has moved.
         *
         * @return The mouse movement delta as a glm::vec2.
         */
        [[nodiscard]] glm::vec2 get_mouse_move_delta() const;

    private:
        input_key sdl_key_to_key(SDL_Scancode sdl_key) const;
        input_key sdl_mouse_to_key(Uint8 button) const;

    private:
        std::unordered_set<input_key> m_current_keys;
        std::unordered_set<input_key> m_previous_keys;
        std::unordered_set<input_key> m_pressed_this_frame;
        std::unordered_set<input_key> m_released_this_frame;

        glm::vec2 m_mouse_pos;
        glm::vec2 m_mouse_delta;
        glm::vec2 m_previous_mouse_pos;
    };

    inline bool input_system::is_key_pressed(input_key k) const {
        return m_pressed_this_frame.find(k) != m_pressed_this_frame.end();
    }

    inline bool input_system::is_key_held(input_key k) const {
        return m_current_keys.find(k) != m_current_keys.end();
    }

    inline bool input_system::is_key_released(input_key k) const {
        return m_released_this_frame.find(k) != m_released_this_frame.end();
    }

    inline glm::vec2 input_system::get_mouse_screen_position() const {
        return m_mouse_pos;
    }

    inline glm::vec2 input_system::get_mouse_move_delta() const {
        return m_mouse_delta;
    }
}  // namespace engine
