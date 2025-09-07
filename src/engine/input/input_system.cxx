#include "input_system.hxx"
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

    void game_input::process_event(const SDL_Event& event) {
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN: {
                input_key k = sdl_key_to_key(event.key.scancode);
                if (k != input_key::none) {
                    if (m_current_keys.find(k) == m_current_keys.end()) {
                        m_pressed_this_frame.insert(k);
                    }
                    m_current_keys.insert(k);
                }
                break;
            }
            case SDL_EVENT_KEY_UP: {
                input_key k = sdl_key_to_key(event.key.scancode);
                if (k != input_key::none) {
                    m_released_this_frame.insert(k);
                    m_current_keys.erase(k);
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                input_key k = sdl_mouse_to_key(event.button.button);
                if (k != input_key::none) {
                    if (m_current_keys.find(k) == m_current_keys.end()) {
                        m_pressed_this_frame.insert(k);
                    }
                    m_current_keys.insert(k);
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                input_key k = sdl_mouse_to_key(event.button.button);
                if (k != input_key::none) {
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

    input_key game_input::sdl_key_to_key(SDL_Scancode scancode) const {
        static const std::unordered_map<SDL_Scancode, input_key> key_map = {
            {SDL_SCANCODE_W, input_key::w},
            {SDL_SCANCODE_A, input_key::a},
            {SDL_SCANCODE_S, input_key::s},
            {SDL_SCANCODE_D, input_key::d},
            {SDL_SCANCODE_C, input_key::c},
            {SDL_SCANCODE_O, input_key::o},
            {SDL_SCANCODE_P, input_key::p},
            {SDL_SCANCODE_G, input_key::g},
            {SDL_SCANCODE_UP, input_key::arrow_up},
            {SDL_SCANCODE_DOWN, input_key::arrow_down},
            {SDL_SCANCODE_LEFT, input_key::arrow_left},
            {SDL_SCANCODE_RIGHT, input_key::arrow_right},
            {SDL_SCANCODE_SPACE, input_key::space},
            {SDL_SCANCODE_ESCAPE, input_key::escape}};

        auto it = key_map.find(scancode);
        return (it != key_map.end()) ? it->second : input_key::none;
    }

    input_key game_input::sdl_mouse_to_key(Uint8 button) const {
        switch (button) {
            case SDL_BUTTON_LEFT:
                return input_key::mouse_left;
            case SDL_BUTTON_RIGHT:
                return input_key::mouse_right;
            case SDL_BUTTON_MIDDLE:
                return input_key::mouse_middle;
            default:
                return input_key::none;
        }
    }

}  // namespace engine
