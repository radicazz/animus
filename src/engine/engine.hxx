#pragma once

#include <type_traits>

#include "engine/version.hxx"
#include "renderer/renderer.hxx"
#include "renderer/camera.hxx"
#include "renderer/viewport.hxx"
#include "utils/window.hxx"
#include "utils/resource_manager.hxx"
#include "utils/input_system.hxx"
#include "ecs/game_entities.hxx"

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
    constexpr bool is_debug_build() {
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
     * @note All callbacks are optional and can be set to `nullptr` if not needed.
     */
    struct game_info {
        void* state = nullptr;

        /**
         * @brief Called in the engine's constructor after initialization.
         */
        void (*on_create)(game_engine*) = nullptr;

        /**
         * @brief Called in the engine's destructor.
         */
        void (*on_destroy)(game_engine*) = nullptr;

        /**
         * @brief Called every frame before rendering.
         */
        void (*on_update)(game_engine*, float delta_time) = nullptr;

        /**
         * @brief Called each tick for consistent physics updates.
         */
        void (*on_fixed_update)(game_engine*, float fixed_delta_time) = nullptr;

        /**
         * @brief Called every frame during rendering.
         */
        void (*on_render)(game_engine*, float interpolation_alpha) = nullptr;
    };

    class game_engine {
    public:
        game_engine(const game_info& info, std::string_view title, const glm::ivec2& size);
        ~game_engine();

        game_engine(const game_engine&) = delete;
        game_engine& operator=(const game_engine&) = delete;
        game_engine(game_engine&&) = delete;
        game_engine& operator=(game_engine&&) = delete;

        /**
         * @brief Start running the game loop.
         * @note This function will block until the game is quit as it runs a while loop.
         */
        void run();

        [[nodiscard]] game_window& get_window();
        [[nodiscard]] game_renderer& get_renderer();
        [[nodiscard]] game_camera& get_camera();
        [[nodiscard]] game_viewport& get_viewport();
        [[nodiscard]] game_resources& get_resources();
        [[nodiscard]] game_input& get_input();
        [[nodiscard]] game_entities& get_entities();

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
        [[nodiscard]] T& get_state();

        static constexpr float ticks_to_time(float ticks_per_second);
        static constexpr float time_to_ticks(float time_seconds);

        [[nodiscard]] float get_tick_rate();

        /**
         * @brief Set the tick rate (fixed update rate) of the engine.
         * @param ticks_per_second The number of fixed updates (ticks) to perform per second.
         */
        void set_tick_rate(float ticks_per_second);

        [[nodiscard]] float get_fixed_delta_time() const;
        [[nodiscard]] float get_delta_time() const;
        [[nodiscard]] float get_interpolation_alpha() const;

    private:
        void process_events();

    private:
        game_window m_window;
        game_renderer m_renderer;
        game_camera m_camera;
        game_viewport m_viewport;
        game_resources m_resources;
        game_input m_input;
        game_entities m_entities;

        game_info m_game;

        /**
         * @brief Whether to keep the game loop running or not.
         * @note Setting this to false will exit the game loop and end the program.
         */
        bool m_is_running;

        /**
         * @brief The amount of time between each fixed update (tick).
         */
        float m_fixed_delta_time;

        /**
         * @brief Fraction of time elapsed towards the next fixed update (tick).
         */
        float m_interpolation_alpha;

        /**
         * @brief The time spent between the last two frames in seconds.
         */
        float m_delta_time;
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

    inline game_viewport& game_engine::get_viewport() {
        return m_viewport;
    }

    inline game_resources& game_engine::get_resources() {
        return m_resources;
    }

    inline game_input& game_engine::get_input() {
        return m_input;
    }

    inline game_entities& game_engine::get_entities() {
        return m_entities;
    }

    template <class T>
        requires std::is_class_v<T>
    inline T& game_engine::get_state() {
        return *static_cast<T*>(m_game.state);
    }

    constexpr float game_engine::ticks_to_time(const float ticks_per_second) {
        return 1.0f / ticks_per_second;
    }

    constexpr float game_engine::time_to_ticks(const float time_seconds) {
        return 1.0f / time_seconds;
    }

    inline float game_engine::get_tick_rate() {
        return time_to_ticks(m_fixed_delta_time);
    }

    inline void game_engine::set_tick_rate(const float ticks_per_second) {
        m_fixed_delta_time = ticks_to_time(ticks_per_second);
    }

    inline float game_engine::get_fixed_delta_time() const {
        return m_fixed_delta_time;
    }

    inline float game_engine::get_delta_time() const {
        return m_delta_time;
    }

    inline float game_engine::get_interpolation_alpha() const {
        return m_interpolation_alpha;
    }
}  // namespace engine
