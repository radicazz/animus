#pragma once

#include "window/window.hxx"

namespace engine {
    class game_engine;

    struct game_callbacks {
        using game_create = void (*)(game_engine*);
        using game_destroy = void (*)(game_engine*);
        using game_process_events = void (*)(game_engine*, SDL_Event*);
        using game_render = void (*)(game_engine*);

        game_create create = nullptr;
        game_destroy destroy = nullptr;
        game_process_events process_events = nullptr;
        game_render render = nullptr;
    };

    class game_engine {
    public:
        game_engine(std::string_view title, glm::vec2 size, void* game_instance,
                    const game_callbacks& callbacks);
        ~game_engine();

        void run();

        window& get_window();
        renderer& get_renderer();

        /**
         * @brief Get a pointer to your game's data.
         *
         * The game communicates with the engine through callbacks. The engine
         * callbacks provide access to the engine's data. In order to avoid
         * a lot of messiness, right now we store a pointer to your game's data
         * in the engine, to access your game's state in callbacks.
         *
         * @tparam T The class that represents your game's state.
         * @return A pointer to your game's data, casted to your game's type.
         */
        template <class T>
        T* get_game_instance();

    private:
        window m_window;
        renderer m_renderer;
        game_callbacks m_callbacks;
        void* m_game;
    };

    inline window& game_engine::get_window() {
        return m_window;
    }

    inline renderer& game_engine::get_renderer() {
        return m_renderer;
    }

    template <class T>
    inline T* game_engine::get_game_instance() {
        return static_cast<T*>(m_game);
    }
}  // namespace engine
