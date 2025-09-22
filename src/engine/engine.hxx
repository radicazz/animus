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

/**
 * @brief The main entry point of the application.
 *        Define this somewhere in your code to start your game.
 * @note You must implement this function in your game code to compile your game.
 */
extern void game_entry_point();

namespace engine {
    class game_engine;

    /**
     * @brief Global callback functions to hook into the game engine lifecycle.
     */
    struct game_engine_callbacks {
        /**
         * @brief Called in the engine's constructor after components are initialized.
         */
        void (*on_engine_start)(game_engine* engine) = nullptr;

        /**
         * @brief Called in the engine's destructor before components are destroyed.
         */
        void (*on_engine_end)(game_engine* engine) = nullptr;

        /**
         * @brief Called every fixed update (tick) at a fixed interval.
         */
        void (*on_engine_tick)(game_engine* engine, float tick_interval) = nullptr;

        /**
         * @brief Called every frame before rendering.
         */
        void (*on_engine_frame)(game_engine* engine, float frame_interval) = nullptr;

        /**
         * @brief Called every frame during rendering.
         */
        void (*on_engine_draw)(game_engine* engine, float fraction_to_next_tick) = nullptr;
    };

    /**
     * @brief The primary game engine class.
     */
    class game_engine {
    public:
        game_engine() = delete;

        /**
         * @brief Construct a new game engine object.
         * @param title The title of the game window.
         * @param size The initial size of the game window.
         * @param callbacks Your game state and its callbacks.
         * @param game_state Pointer to your game's state data.
         */
        game_engine(std::string_view title, const glm::ivec2& size,
                    const game_engine_callbacks& callbacks, void* game_state);
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

        [[nodiscard]] game_window* get_window() noexcept;
        [[nodiscard]] game_renderer* get_renderer() noexcept;
        [[nodiscard]] game_input* get_input() noexcept;
        [[nodiscard]] game_scenes* get_scenes() noexcept;

        /**
         * @brief Get a pointer to your game's data.
         * @tparam T The type of your game's state data. Must be a class type.
         * @return Pointer to your game's state data as type T.
         */
        template <class T>
            requires std::is_class_v<T>
        [[nodiscard]] T* get_state() noexcept;

        [[nodiscard]] float get_tick_rate() noexcept;
        void set_tick_rate(float tick_rate_seconds);

        [[nodiscard]] float get_tick_interval() const noexcept;
        [[nodiscard]] float get_fraction_to_next_tick() const noexcept;
        [[nodiscard]] float get_frame_interval() const noexcept;

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

        void* m_game_state;
        game_engine_callbacks m_game_callbacks;

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

    inline game_window* game_engine::get_window() noexcept {
        return m_window.get();
    }

    inline game_renderer* game_engine::get_renderer() noexcept {
        return m_renderer.get();
    }

    inline game_input* game_engine::get_input() noexcept {
        return m_input.get();
    }

    inline game_scenes* game_engine::get_scenes() noexcept {
        return m_scenes.get();
    }

    template <class T>
        requires std::is_class_v<T>
    T* game_engine::get_state() noexcept {
        return static_cast<T*>(m_game_state);
    }

    inline float game_engine::get_tick_rate() noexcept {
        return ticks_rate_to_interval(m_tick_interval_seconds);
    }

    inline void game_engine::set_tick_rate(float tick_rate_seconds) {
        m_tick_interval_seconds = ticks_rate_to_interval(tick_rate_seconds);
    }

    inline float game_engine::get_tick_interval() const noexcept {
        return m_tick_interval_seconds;
    }

    inline float game_engine::get_fraction_to_next_tick() const noexcept {
        return m_fraction_to_next_tick;
    }

    inline float game_engine::get_frame_interval() const noexcept {
        return m_frame_interval_seconds;
    }
}  // namespace engine
