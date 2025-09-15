#include "input.hxx"

#include <unordered_map>

namespace engine {
    game_input::game_input() : m_mouse_pos(0, 0), m_mouse_delta(0, 0), m_previous_mouse_pos(0, 0) {
    }

    void game_input::update() {
        // Clear frame-specific states
        m_pressed_this_frame.clear();
        m_released_this_frame.clear();

        // Update mouse delta
        m_mouse_delta = m_mouse_pos - m_previous_mouse_pos;
        m_previous_mouse_pos = m_mouse_pos;

        // Move current to previous
        m_previous_keys = m_current_keys;
    }

    void game_input::process_sdl_event(const SDL_Event& event) {
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN: {
                game_input_key k = sdl_key_to_input_key(event.key.scancode);
                if (k != game_input_key::unknown) {
                    if (m_current_keys.find(k) == m_current_keys.end()) {
                        m_pressed_this_frame.insert(k);
                    }
                    m_current_keys.insert(k);
                }
                break;
            }
            case SDL_EVENT_KEY_UP: {
                game_input_key k = sdl_key_to_input_key(event.key.scancode);
                if (k != game_input_key::unknown) {
                    m_released_this_frame.insert(k);
                    m_current_keys.erase(k);
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                game_input_key k = sdl_mouse_to_input_key(event.button.button);
                if (k != game_input_key::unknown) {
                    if (m_current_keys.find(k) == m_current_keys.end()) {
                        m_pressed_this_frame.insert(k);
                    }
                    m_current_keys.insert(k);
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                game_input_key k = sdl_mouse_to_input_key(event.button.button);
                if (k != game_input_key::unknown) {
                    m_released_this_frame.insert(k);
                    m_current_keys.erase(k);
                }
                break;
            }
            case SDL_EVENT_MOUSE_MOTION: {
                m_mouse_pos = {event.motion.x, event.motion.y};
                break;
            }
        }
    }

    glm::vec2 game_input::get_movement_wasd() const {
        glm::vec2 movement(0.0f);

        if (is_key_held(game_input_key::w)) {
            movement.y -= 1.0f;
        }

        if (is_key_held(game_input_key::s)) {
            movement.y += 1.0f;
        }

        if (is_key_held(game_input_key::a)) {
            movement.x -= 1.0f;
        }

        if (is_key_held(game_input_key::d)) {
            movement.x += 1.0f;
        }

        // Normalize to prevent faster diagonal movement
        if (movement != glm::vec2(0.0f)) {
            movement = glm::normalize(movement);
        }

        return movement;
    }

    glm::vec2 game_input::get_movement_arrows() const {
        glm::vec2 movement(0.0f);

        if (is_key_held(game_input_key::arrow_up)) {
            movement.y -= 1.0f;
        }

        if (is_key_held(game_input_key::arrow_down)) {
            movement.y += 1.0f;
        }

        if (is_key_held(game_input_key::arrow_left)) {
            movement.x -= 1.0f;
        }

        if (is_key_held(game_input_key::arrow_right)) {
            movement.x += 1.0f;
        }

        // Normalize to prevent faster diagonal movement
        if (movement != glm::vec2(0.0f)) {
            movement = glm::normalize(movement);
        }

        return movement;
    }
}  // namespace engine
