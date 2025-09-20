/**
 * @file engine.hxx
 * @brief Main game engine header file.
 *
 * This file defines the `engine::game_engine` class and its associated types and functions.
 */

#pragma once

#include "logger.hxx"
#include "renderer/renderer.hxx"
#include "renderer/camera.hxx"
#include "renderer/viewport.hxx"
#include "utils/window.hxx"
#include "utils/resources.hxx"
#include "utils/input.hxx"
#include "utils/scenes.hxx"
#include "ecs/entities.hxx"
#include "utils/timing.hxx"

namespace engine {
    class game_engine;

    /**
     * @brief State pointer and callback functions for the game engine.
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
        [[nodiscard]] game_input& get_input();
        [[nodiscard]] game_scenes& get_scenes();

        /**
         * @brief Get a pointer to your game's data.
         * @tparam T The type of your game's state data. Must be a class type.
         * @return Reference to your game's state data as type T.
         */
        template <class T>
            requires std::is_class_v<T>
        [[nodiscard]] T& get_state();

        [[nodiscard]] float get_tick_rate();
        void set_tick_rate(float tick_rate_seconds);

        [[nodiscard]] float get_tick_interval() const;
        [[nodiscard]] float get_fraction_to_next_tick() const;
        [[nodiscard]] float get_frame_interval() const;

    private:
        /**
         * @brief Internal wrapper to initialize and shutdown SDL and related subsystems.
         *
         * The game_engine class initalizes all of its subsystems (window, renderer, input, etc)
         * before it enters its own constructor due to the design of initializer lists in C++.
         *
         * By making this the first member and initializing it first, it gives us a chance to
         * run some code before the rest of the engine is initialized.
         */
        struct engine_wrapper {
            engine_wrapper();
            ~engine_wrapper();
        } m_wrapper;

        /**
         * @brief Whether to keep the game loop running or not.
         * @note Setting this to false will exit the game loop and end the program.
         */
        bool m_is_running;

        game_info m_game;

        std::unique_ptr<game_window> m_window;
        std::unique_ptr<game_renderer> m_renderer;
        std::unique_ptr<game_input> m_input;
        std::unique_ptr<game_scenes> m_scenes;

        /**
         * @brief The amount of time between each fixed update (tick).
         */
        float m_tick_interval_seconds;

        /**
         * @brief Fraction of time elapsed towards the next fixed update (tick).
         */
        float m_fraction_to_next_tick;

        /**
         * @brief The time spent between the last two frames in seconds.
         */
        float m_frame_interval_seconds;
    };

    inline [[nodiscard]] game_window& game_engine::get_window() {
        return *m_window;
    }

    inline [[nodiscard]] game_renderer& game_engine::get_renderer() {
        return *m_renderer;
    }

    inline [[nodiscard]] game_input& game_engine::get_input() {
        return *m_input;
    }

    inline [[nodiscard]] game_scenes& game_engine::get_scenes() {
        return *m_scenes;
    }

    template <class T>
        requires std::is_class_v<T>
    [[nodiscard]] T& game_engine::get_state() {
        return *static_cast<T*>(m_game.state);
    }

    inline [[nodiscard]] float game_engine::get_tick_rate() {
        return ticks_rate_to_interval(m_tick_interval_seconds);
    }

    inline void game_engine::set_tick_rate(float tick_rate_seconds) {
        m_tick_interval_seconds = ticks_rate_to_interval(tick_rate_seconds);
    }

    inline [[nodiscard]] float game_engine::get_tick_interval() const {
        return m_tick_interval_seconds;
    }

    inline [[nodiscard]] float game_engine::get_fraction_to_next_tick() const {
        return m_fraction_to_next_tick;
    }

    inline [[nodiscard]] float game_engine::get_frame_interval() const {
        return m_frame_interval_seconds;
    }
}  // namespace engine
