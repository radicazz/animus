/**
 * @file engine.hxx
 * @brief Main game engine header file.
 *
 * this file defines the `engine::game_engine` class and its associated types and functions that
 * manage the core game loop, window, rendering, input, resources, and entities.
 */

#pragma once

#include "logger.hxx"
#include "engine/version.hxx"
#include "renderer/renderer.hxx"
#include "renderer/camera.hxx"
#include "renderer/viewport.hxx"
#include "utils/window.hxx"
#include "utils/resources.hxx"
#include "utils/input.hxx"
#include "ecs/entities.hxx"

namespace engine {
    /**
     * @brief Check if the current build is a debug build.
     * @return True if this is a debug build, false otherwise.
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
     * @brief State pointer and callback functions for the game engine.
     *
     * These functions are called at various points in the game loop. Your game must implement these
     * functions to hook into the engine's lifecycle. Each function provides an
     * `engine::game_engine` pointer as its first argument, allowing the game to access the
     * engine's functionality.
     *
     * @note All callbacks are optional and can be set to `nullptr` if not needed.
     */
    struct game_info {
        /**
         * @brief User-defined state data for the game.
         */
        void* state = nullptr;

        /**
         * @brief Called in the engine's constructor after components are initialized.
         */
        void (*on_create)(game_engine*) = nullptr;

        /**
         * @brief Called in the engine's destructor before components are destroyed.
         */
        void (*on_destroy)(game_engine*) = nullptr;

        /**
         * @brief Called every fixed update (tick) at a fixed interval.
         * @param tick_interval The fixed time interval between ticks in seconds.
         * @note The default tick rate is 32 ticks per second. Use `game_engine::set_tick_rate` to
         * set a custom tick rate.
         */
        void (*on_tick)(game_engine*, float tick_interval) = nullptr;

        /**
         * @brief Called every frame before rendering.
         * @param frame_interval The time spent between the last two frames in seconds.
         * @note This function is called at a variable rate depending on the performance of the
         * game.
         */
        void (*on_frame)(game_engine*, float frame_interval) = nullptr;

        /**
         * @brief Called every frame during rendering.
         * @param fraction_to_next_tick A value between 0.0 and 1.0 representing the progress
         *        of the current frame towards the next tick.
         * @note This function is called after `on_frame` and is where all rendering should be done.
         */
        void (*on_draw)(game_engine*, float fraction_to_next_tick) = nullptr;
    };

    /**
     * @brief The primary game engine class.
     *
     * This class manages the main game loop, window, rendering, input, resources, and entities.
     * It provides access to all core engine functionality and serves as the primary interface
     * between your game and the engine.
     *
     * @code
     * #include <SDL/SDL_main.h>
     * #include "engine/engine.hxx"
     *
     * struct example_game_state {
     *    int frame_count;
     * };
     *
     * void example_game_frame(engine::game_engine* engine, float frame_interval) {
     *   auto& state = engine->get_state<example_game_state>();
     *
     *   // Update your game state here.
     *   state.frame_count++;
     * }
     *
     * int main(int argc, char* argv[]) {
     *   try {
     *      example_game_state state;
     *      state.frame_count = 0;
     *
     *      engine::game_info info = {
     *       .state = &state,
     *       .on_create = nullptr,
     *       .on_destroy = nullptr,
     *       .on_tick = nullptr,
     *       .on_frame = example_game_frame,
     *       .on_draw = nullptr
     *      };
     *
     *      engine::game_engine game(info, "Example Game", { 800, 600 });
     *
     *      //  Run the game.
     *      game.run();
     *   } catch (const std::exception& error) {
     *      // Handle initialization errors.
     *  }
     *
     *    return 0;
     * }
     * @endcode
     */
    class game_engine {
    public:
        game_engine() = delete;

        /**
         * @brief Construct a new game engine object.
         * @param info Your game state and its callbacks.
         * @param title The initial title of the game window.
         * @param size The initial size of the game window.
         */
        game_engine(const game_info& info, std::string_view title, const glm::ivec2& size);
        ~game_engine();

        game_engine(const game_engine&) = delete;
        game_engine& operator=(const game_engine&) = delete;
        game_engine(game_engine&&) = delete;
        game_engine& operator=(game_engine&&) = delete;

        /**
         * @brief Start running the game loop.
         * @note This function will block until the game quits.
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
         * @tparam T The type of your game's state data. Must be a class type.
         * @return Reference to your game's state data as type T.
         *
         * @code
         * struct my_game_state {
         *     int score;
         * };
         *
         * void on_create(engine::game_engine* engine) {
         *    // Access your game's state through the engine.
         *     auto& state = engine->get_state<my_game_state>();
         *     state.score = 0;
         * }
         * @endcode
         */
        template <class T>
            requires std::is_class_v<T>
        [[nodiscard]] T& get_state();

        static constexpr float tick_rate_to_interval(float tick_rate_seconds);
        static constexpr float tick_interval_to_rate(float tick_interval_seconds);

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

    constexpr float game_engine::tick_interval_to_rate(const float tick_interval_seconds) {
        return 1.0f / tick_interval_seconds;
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
