#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <unordered_set>

namespace engine {
    enum class game_input_key {
        none,
        w,
        a,
        s,
        d,
        c,
        o,
        p,
        g,
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

    enum class game_input_state { pressed, held, released };

    class game_input {
    public:
        game_input();

        // Rule of 5 - using defaults since no resource management
        game_input(const game_input&) = default;
        game_input& operator=(const game_input&) = default;
        game_input(game_input&&) = default;
        game_input& operator=(game_input&&) = default;
        ~game_input() = default;

        void update();
        void process_sdl_event(const SDL_Event& event);

        [[nodiscard]] bool is_key_pressed(game_input_key key) const;
        [[nodiscard]] bool is_key_held(game_input_key key) const;
        [[nodiscard]] bool is_key_released(game_input_key key) const;

        // TODO: Need to make an input mapping system for this...
        // but for now it will suffice.

        [[nodiscard]] glm::vec2 get_movement_wasd() const;
        [[nodiscard]] glm::vec2 get_movement_arrows() const;

        /**
         * @brief Get the current mouse position in screen space.
         * @return The mouse position as a glm::vec2.
         */
        [[nodiscard]] glm::vec2 get_mouse_position() const;

        /**
         * @brief Get the mouse movement delta since the last frame.
         *
         * This function returns the change in mouse position (delta) since the last frame,
         * allowing you to track how much the mouse has moved.
         *
         * @return The mouse movement delta as a glm::vec2.
         */
        [[nodiscard]] glm::vec2 get_mouse_movement() const;

    private:
        game_input_key sdl_key_to_key(SDL_Scancode sdl_key) const;
        game_input_key sdl_mouse_to_key(Uint8 button) const;

    private:
        std::unordered_set<game_input_key> m_current_keys;
        std::unordered_set<game_input_key> m_previous_keys;
        std::unordered_set<game_input_key> m_pressed_this_frame;
        std::unordered_set<game_input_key> m_released_this_frame;

        glm::vec2 m_mouse_pos;
        glm::vec2 m_mouse_delta;
        glm::vec2 m_previous_mouse_pos;
    };

    inline bool game_input::is_key_pressed(game_input_key k) const {
        return m_pressed_this_frame.find(k) != m_pressed_this_frame.end();
    }

    inline bool game_input::is_key_held(game_input_key k) const {
        return m_current_keys.find(k) != m_current_keys.end();
    }

    inline bool game_input::is_key_released(game_input_key k) const {
        return m_released_this_frame.find(k) != m_released_this_frame.end();
    }

    inline glm::vec2 game_input::get_mouse_position() const {
        return m_mouse_pos;
    }

    inline glm::vec2 game_input::get_mouse_movement() const {
        return m_mouse_delta;
    }
}  // namespace engine
