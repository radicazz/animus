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

    void game_input::process_sdl_event(const SDL_Event& event) {
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN: {
                game_input_key k = sdl_key_to_key(event.key.scancode);
                if (k != game_input_key::none) {
                    if (m_current_keys.find(k) == m_current_keys.end()) {
                        m_pressed_this_frame.insert(k);
                    }
                    m_current_keys.insert(k);
                }
                break;
            }
            case SDL_EVENT_KEY_UP: {
                game_input_key k = sdl_key_to_key(event.key.scancode);
                if (k != game_input_key::none) {
                    m_released_this_frame.insert(k);
                    m_current_keys.erase(k);
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                game_input_key k = sdl_mouse_to_key(event.button.button);
                if (k != game_input_key::none) {
                    if (m_current_keys.find(k) == m_current_keys.end()) {
                        m_pressed_this_frame.insert(k);
                    }
                    m_current_keys.insert(k);
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                game_input_key k = sdl_mouse_to_key(event.button.button);
                if (k != game_input_key::none) {
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

    game_input_key game_input::sdl_key_to_key(SDL_Scancode scancode) const {
        static const std::unordered_map<SDL_Scancode, game_input_key> key_map = {
            {SDL_SCANCODE_W, game_input_key::w},
            {SDL_SCANCODE_A, game_input_key::a},
            {SDL_SCANCODE_S, game_input_key::s},
            {SDL_SCANCODE_D, game_input_key::d},
            {SDL_SCANCODE_C, game_input_key::c},
            {SDL_SCANCODE_O, game_input_key::o},
            {SDL_SCANCODE_P, game_input_key::p},
            {SDL_SCANCODE_G, game_input_key::g},
            {SDL_SCANCODE_UP, game_input_key::arrow_up},
            {SDL_SCANCODE_DOWN, game_input_key::arrow_down},
            {SDL_SCANCODE_LEFT, game_input_key::arrow_left},
            {SDL_SCANCODE_RIGHT, game_input_key::arrow_right},
            {SDL_SCANCODE_SPACE, game_input_key::space},
            {SDL_SCANCODE_ESCAPE, game_input_key::escape}};

        auto it = key_map.find(scancode);
        return (it != key_map.end()) ? it->second : game_input_key::none;
    }

    game_input_key game_input::sdl_mouse_to_key(Uint8 button) const {
        switch (button) {
            case SDL_BUTTON_LEFT:
                return game_input_key::mouse_left;
            case SDL_BUTTON_RIGHT:
                return game_input_key::mouse_right;
            case SDL_BUTTON_MIDDLE:
                return game_input_key::mouse_middle;
            default:
                return game_input_key::none;
        }
    }

}  // namespace engine
