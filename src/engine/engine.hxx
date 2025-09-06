#pragma once

#include <type_traits>

#include "engine/version.hxx"
#include "camera/camera.hxx"
#include "input/input_system.hxx"
#include "renderer/renderer.hxx"
#include "window/window.hxx"
#include "utils/resource_manager.hxx"

namespace engine {
    /**
     * @brief Check if the current build is a debug build.
     *
     * @example
     * if constexpr (is_debug_build() == true) {
     *     // Debug-specific code here
     * }
     *
     * @return consteval bool True if this is a debug build, false otherwise.
     */
    consteval bool is_debug_build() {
#ifdef NDEBUG
        return false;
#else
        return true;
#endif
    }

    class game_engine;

    /**
     * @brief Callback functions for the game engine.
     *
     * These functions are called at various points in the game loop. Your game must implement these
     * functions to hook into the engine's lifecycle. Each function provides an
     * `engine::game_engine` pointer as its first argument, allowing access to the engine's
     * functionality.
     *
     * These functions are called in the following order:
     * 1. `create`
     * 2. `update`
     * 3. `render`
     * 4. `destroy`
     *
     */
    struct game_callbacks {
        /**
         * @brief Called last in the engine's constructor.
         *
         * Use this as an opportunity to initialize your game state outside of it's own constructor.
         * For example, load sprites, fonts and other resources here.
         *
         */
        void (*on_create)(game_engine*) = nullptr;

        /**
         * @brief Called in the engine's constructor.
         *
         * RAII handles most of the cleanup, so you typically don't need to do anything here.
         *
         */
        void (*on_destroy)(game_engine*) = nullptr;

        /**
         * @brief Called every frame before rendering.
         *
         * @note This callback has an extra parameter, `float delta_time`, which represents the time
         *       elapsed since the last frame.
         */
        void (*on_update)(game_engine*, float delta_time) = nullptr;

        /**
         * @brief Called every frame during rendering.
         *
         */
        void (*on_render)(game_engine*) = nullptr;
    };

    /**
     * @brief Details for initializing the game engine.
     *
     * This structure contains parameters used to configure the game engine during its construction.
     * It includes settings for the initial window title and size.
     */
    struct game_details {
        std::string_view window_title;
        glm::vec2 window_size;
    };

    class game_engine {
    public:
        game_engine(const game_details& details, void* state, const game_callbacks& callbacks);
        ~game_engine();

        /**
         * @brief Start running the game loop.
         * @note This function will block until the game is quit as it runs a while loop.
         */
        void run();

        [[nodiscard]] game_window& get_window();
        [[nodiscard]] game_renderer& get_renderer();
        [[nodiscard]] game_camera& get_camera();
        [[nodiscard]] resource_manager& get_resource_manager();
        [[nodiscard]] input_system& get_input_system();

        /**
         * @brief Get a pointer to your game's data.
         *
         * The game communicates with the engine through callbacks. The engine
         * callbacks provide access to the engine's data.
         *
         * Provide a pointer to your game's data in the constructor to access it through the engine
         * in your callbacks.
         *
         * @tparam T The class that represents your game's state.
         * @return A pointer to your game's data, casted to your game's type.
         *
         * @code
         * struct my_game_state {
         *     int score;
         *     glm::vec2 player_position;
         * };
         *
         * void on_create(engine::game_engine* engine) {
         *    // Access your game's state through the engine.
         *     auto& state = engine->get_state<my_game_state>();
         *     state.score = 0;
         *     state.player_position = { 0.0f, 0.0f };
         * }
         * @endcode
         */
        template <class T>
            requires std::is_class_v<T>
        T& get_state();

    private:
        game_window m_window;
        game_renderer m_renderer;
        game_camera m_camera;
        resource_manager m_resource_manager;
        input_system m_input_system;

        game_callbacks m_callbacks;
        void* m_state;
    };

    inline game_window& game_engine::get_window() {
        return m_window;
    }

    inline game_renderer& game_engine::get_renderer() {
        return m_renderer;
    }

    inline game_camera& game_engine::get_camera() {
        return m_camera;
    }

    inline resource_manager& game_engine::get_resource_manager() {
        return m_resource_manager;
    }

    inline input_system& game_engine::get_input_system() {
        return m_input_system;
    }

    template <class T>
        requires std::is_class_v<T>
    inline T& game_engine::get_state() {
        return *static_cast<T*>(m_state);
    }
}  // namespace engine
