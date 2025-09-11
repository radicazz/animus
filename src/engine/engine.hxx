#pragma once

#include <type_traits>

#include "logger.hxx"
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
         * @brief Called at a fixed tick rate for game logic updates.
         */
        void (*on_tick)(game_engine*, float tick_interval) = nullptr;

        /**
         * @brief Called every frame before rendering.
         */
        void (*on_frame)(game_engine*, float frame_interval) = nullptr;

        /**
         * @brief Called every frame during rendering.
         */
        void (*on_draw)(game_engine*, float progress_to_next_tick) = nullptr;
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

        static constexpr float tick_rate_to_interval(float tick_rate_seconds);
        static constexpr float tick_interval_to_rate(float time_seconds);

        [[nodiscard]] float get_tick_rate();
        void set_tick_rate(float tick_rate_seconds);

        [[nodiscard]] float get_tick_interval() const;
        [[nodiscard]] float get_fraction_to_next_tick() const;

        [[nodiscard]] float get_frame_interval() const;

    private:
        void process_events();

    private:
        game_info m_game;
        game_window m_window;
        game_renderer m_renderer;
        game_resources m_resources;
        game_input m_input;
        game_entities m_entities;
        game_camera m_camera;
        game_viewport m_viewport;

        /**
         * @brief Whether to keep the game loop running or not.
         * @note Setting this to false will exit the game loop and end the program.
         */
        bool m_is_running;

        /**
         * @brief The amount of time between each fixed update (tick).
         */
        float m_tick_interval;

        /**
         * @brief Fraction of time elapsed towards the next fixed update (tick).
         */
        float m_fraction_to_next_tick;

        /**
         * @brief The time spent between the last two frames in seconds.
         */
        float m_frame_interval;
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

    constexpr float game_engine::tick_rate_to_interval(const float ticks_per_second) {
        return 1.0f / ticks_per_second;
    }

    constexpr float game_engine::tick_interval_to_rate(const float time_seconds) {
        return 1.0f / time_seconds;
    }

    inline float game_engine::get_tick_rate() {
        return tick_interval_to_rate(m_tick_interval);
    }

    inline void game_engine::set_tick_rate(const float ticks_per_second) {
        m_tick_interval = tick_rate_to_interval(ticks_per_second);
    }

    inline float game_engine::get_tick_interval() const {
        return m_tick_interval;
    }

    inline float game_engine::get_frame_interval() const {
        return m_frame_interval;
    }

    inline float game_engine::get_fraction_to_next_tick() const {
        return m_fraction_to_next_tick;
    }
}  // namespace engine
