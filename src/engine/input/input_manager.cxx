#include "input_manager.hxx"

namespace engine {
    input_manager::input_manager()
        : m_mouse_pos(0, 0), m_mouse_delta(0, 0), m_previous_mouse_pos(0, 0) {
    }

    void input_manager::update() {
        // Clear frame-specific states
        m_pressed_this_frame.clear();
        m_released_this_frame.clear();

        // Update mouse delta
        m_mouse_delta = m_mouse_pos - m_previous_mouse_pos;
        m_previous_mouse_pos = m_mouse_pos;

        // Move current to previous
        m_previous_keys = m_current_keys;
    }

    void input_manager::process_event(const SDL_Event& event) {
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

    glm::vec2 input_manager::get_movement_input() const {
        glm::vec2 movement(0.0f);

        // TODO: Add arrow keys.
        if (is_key_held(input_key::w))
            movement.y -= 1.0f;
        if (is_key_held(input_key::s))
            movement.y += 1.0f;
        if (is_key_held(input_key::a))
            movement.x -= 1.0f;
        if (is_key_held(input_key::d))
            movement.x += 1.0f;

        return movement;
    }

    input_key input_manager::sdl_key_to_key(SDL_Scancode scancode) const {
        static const std::unordered_map<SDL_Scancode, input_key> key_map = {
            {SDL_SCANCODE_W, input_key::w},         {SDL_SCANCODE_A, input_key::a},
            {SDL_SCANCODE_S, input_key::s},         {SDL_SCANCODE_D, input_key::d},
            {SDL_SCANCODE_SPACE, input_key::space}, {SDL_SCANCODE_ESCAPE, input_key::escape}};

        auto it = key_map.find(scancode);
        return (it != key_map.end()) ? it->second : input_key::none;
    }

    input_key input_manager::sdl_mouse_to_key(Uint8 button) const {
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
