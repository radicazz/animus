#include "engine.hxx"

#include "logger.hxx"

#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>

namespace engine {
    template <class... Args>
    void try_invoke_callback(void (*func)(Args...), Args... args) {
        if (func != nullptr) {
            func(args...);
        }
    }

    game_engine::game_engine(const game_info& info, std::string_view title, const glm::ivec2& size)
        : m_wrapper(),
          m_is_running(true),
          m_game(info),
          m_window(title, size, game_window_type::resizable),
          m_renderer(m_window.get_sdl_window()),
          m_resources(m_renderer),
          m_input(),
          m_entities(),
          m_camera({0.f, 0.f}, 1.f),
          m_viewport({1.f, 1.f}),  // Full window coverage in normalized coords
          m_tick_interval_seconds(-1.f),
          m_fraction_to_next_tick(-1.f),
          m_frame_interval_seconds(-1.f) {
        // Set a default icon, can be overridden later.
        m_window.set_icon("assets/icons/default");

        set_tick_rate(32.f);

        // TODO: Figure out what to do with these?
        m_renderer.set_camera(&m_camera);
        m_renderer.set_viewport(&m_viewport);

        try_invoke_callback(m_game.on_create, this);

        if (m_game.state == nullptr) {
            log_warning("Game state is null. Did you forget to set it in game_info?");
        }
    }

    game_engine::~game_engine() {
        try_invoke_callback(m_game.on_destroy, this);
    }

    void game_engine::run() {
        std::uint64_t frame_performance_count = performance_counter_value_current();
        float senconds_since_last_tick = 0.f;

        while (m_is_running == true) {
            m_frame_interval_seconds = performance_counter_seconds_since(frame_performance_count);
            frame_performance_count = performance_counter_value_current();
            senconds_since_last_tick += m_frame_interval_seconds;

            process_events();

            while (senconds_since_last_tick >= m_tick_interval_seconds) [[likely]] {
                try_invoke_callback(m_game.on_tick, this, m_tick_interval_seconds);
                senconds_since_last_tick -= m_tick_interval_seconds;
            }

            m_fraction_to_next_tick = senconds_since_last_tick / m_tick_interval_seconds;

            try_invoke_callback(m_game.on_frame, this, m_frame_interval_seconds);

            m_renderer.draw_begin();
            try_invoke_callback(m_game.on_draw, this, m_fraction_to_next_tick);
            m_renderer.draw_end();
        }
    }

    void game_engine::process_events() {
        m_input.update();

        SDL_Event event;
        while (SDL_PollEvent(&event) == true) {
            if (event.type == SDL_EVENT_QUIT) {
                m_is_running = false;
            }

            m_input.process_sdl_event(event);
        }
    }

    game_engine::engine_wrapper::engine_wrapper() {
        log_info("\n");
        log_info("Project '{}' (v{} {}) starting up...", project_name, version::full, build_type);

        if (SDL_Init(SDL_INIT_VIDEO) == false) {
            throw std::runtime_error("Failed to initialize SDL.");
        }

        log_info("SDL initialized successfully: v{}.{}.{}", SDL_MAJOR_VERSION, SDL_MINOR_VERSION,
                 SDL_MICRO_VERSION);

        if (TTF_Init() == false) {
            SDL_Quit();
            throw std::runtime_error("Failed to initialize SDL_ttf.");
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
