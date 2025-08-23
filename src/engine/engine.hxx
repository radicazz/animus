#pragma once

#include <type_traits>

#include "camera/camera.hxx"
#include "input/input_system.hxx"
#include "renderer/renderer.hxx"
#include "window/window.hxx"
#include "utils/resource_manager.hxx"

namespace engine {
    class game_engine;

    struct game_callbacks {
        /**
         * @brief Called before the game loop starts.
         *
         * Use this as an opportunity to initialize your game state outside of it's own constructor.
         * Think of it like Unity's Start method, as opposed to the contructor being the Awake
         * method.
         *
         */
        void (*create)(game_engine*) = nullptr;

        /**
         * @brief Called after the game loop, before the engine shuts down.
         *
         */
        void (*destroy)(game_engine*) = nullptr;

        /**
         * @brief Called every frame before rendering.
         *
         */
        void (*update)(game_engine*, float delta_time) = nullptr;

        /**
         * @brief Called every frame during rendering.
         *
         */
        void (*render)(game_engine*) = nullptr;
    };

    struct game_details {
        std::string_view title;
        glm::vec2 size;
    };

    class game_engine {
    public:
        game_engine(const game_details& details, void* state, const game_callbacks& callbacks);
        ~game_engine();

        void run();

        [[nodiscard]] window& get_window();
        [[nodiscard]] renderer& get_renderer();
        [[nodiscard]] camera& get_camera();
        [[nodiscard]] resource_manager& get_resource_manager();
        [[nodiscard]] input_system& get_input_system();

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
            requires std::is_class_v<T>
        T& get_state();

    private:
        window m_window;
        renderer m_renderer;
        camera m_camera;
        resource_manager m_resource_manager;
        input_system m_input;

        game_callbacks m_callbacks;
        void* m_state;
    };

    inline window& game_engine::get_window() {
        return m_window;
    }

    inline renderer& game_engine::get_renderer() {
        return m_renderer;
    }

    inline camera& game_engine::get_camera() {
        return m_camera;
    }

    inline resource_manager& game_engine::get_resource_manager() {
        return m_resource_manager;
    }

    inline input_system& game_engine::get_input_system() {
        return m_input;
    }

    template <class T>
        requires std::is_class_v<T>
    inline T& game_engine::get_state() {
        return *static_cast<T*>(m_state);
    }
}  // namespace engine
