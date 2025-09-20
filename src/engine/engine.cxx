#include "engine.hxx"

#include <stdexcept>

#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "logger.hxx"
#include "safety.hxx"

namespace engine {
    game_engine::game_engine(const game_info& info, std::string_view title, const glm::ivec2& size)
        : m_wrapper(),
          m_is_running(true),
          m_game(info),
          m_window(std::make_unique<game_window>(title, size, game_window_type::resizable)),
          m_renderer(std::make_unique<game_renderer>(m_window->get_sdl_window())),
          m_input(std::make_unique<game_input>()),
          m_scenes(std::make_unique<game_scenes>(this)),
          m_tick_interval_seconds(-1.f),
          m_fraction_to_next_tick(-1.f),
          m_frame_interval_seconds(-1.f) {
        // Set a default icon, can be overridden later.
        m_window->set_icon("assets/icons/default");

        // Set the default tick rate.
        set_tick_rate(32.f);

        // Let the game know it has been created.
        safe_invoke(m_game.on_create, this);
    }

    game_engine::~game_engine() {
        // Runs before everything is torn down.
        safe_invoke(m_game.on_destroy, this);
    }

    void game_engine::run() {
        std::uint64_t frame_performance_count = performance_counter_value_current();
        float seconds_since_last_tick = 0.f;

        while (m_is_running == true) {
            m_frame_interval_seconds = performance_counter_seconds_since(frame_performance_count);
            frame_performance_count = performance_counter_value_current();
            seconds_since_last_tick += m_frame_interval_seconds;

            {
                m_input->update();

                SDL_Event event;
                while (SDL_PollEvent(&event) == true) {
                    if (event.type == SDL_EVENT_QUIT) {
                        m_is_running = false;
                    }

                    m_input->process_sdl_event(event);
                }
            }

            m_scenes->on_input();

            while (seconds_since_last_tick >= m_tick_interval_seconds) [[likely]] {
                safe_invoke(m_game.on_tick, this, m_tick_interval_seconds);
                m_scenes->on_tick(m_tick_interval_seconds);
                seconds_since_last_tick -= m_tick_interval_seconds;
            }

            m_fraction_to_next_tick = seconds_since_last_tick / m_tick_interval_seconds;

            safe_invoke(m_game.on_frame, this, m_frame_interval_seconds);
            m_scenes->on_frame(m_frame_interval_seconds);

            m_renderer->draw_begin();
            m_scenes->on_draw(m_fraction_to_next_tick);
            safe_invoke(m_game.on_draw, this, m_fraction_to_next_tick);
            m_renderer->draw_end();
        }
    }

    game_engine::engine_wrapper::engine_wrapper() {
        log_info("\n");
        log_info("Project '{}' (v{} {}) starting up...", project_name, version::full, build_type);

        if (SDL_Init(SDL_INIT_VIDEO) == false) {
            throw error_message("Failed to initialize SDL. {}", SDL_GetError());
        }

        log_info("SDL initialized successfully: v{}.{}.{}", SDL_MAJOR_VERSION, SDL_MINOR_VERSION,
                 SDL_MICRO_VERSION);

        if (TTF_Init() == false) {
            SDL_Quit();
            throw error_message("Failed to initialize SDL_ttf.");
        }

        log_info("TTF initialized successfully: v{}.{}.{}", SDL_TTF_MAJOR_VERSION,
                 SDL_TTF_MINOR_VERSION, SDL_TTF_MICRO_VERSION);
    }

    game_engine::engine_wrapper::~engine_wrapper() {
        TTF_Quit();
        log_info("TTF shut down.");

        SDL_Quit();
        log_info("SDL shut down.");
    }
}  // namespace engine

/**
 * @brief The main entry point of the application.
 *
 * Define this somewhere in your code to start your game. Not sure if this is the best way to
 * handle this, but it works for now.
 */
extern void game_start();

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    try {
        game_start();
    } catch (const std::exception& e) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", e.what(), nullptr);
        return 1;
    }

    return 0;
}
