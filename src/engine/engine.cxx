#include "engine.hxx"
#include "logger.hxx"

#include <stdexcept>

namespace engine {
    template <class... Args>
    void try_invoke_callback(void (*func)(Args...), Args... args) {
        if (func != nullptr) {
            func(args...);
        }
    }

    game_engine::game_engine(const game_info& info, std::string_view title, const glm::ivec2& size)
        : m_game(info),
          m_window(title, size),
          m_renderer(m_window.get_sdl_window()),
          m_resources(m_renderer),
          m_input(),
          m_entities(),
          m_camera({0.f, 0.f}, 1.f),
          m_viewport(size),
          m_is_running(true),
          m_tick_interval(-1.f),
          m_fraction_to_next_tick(-1.f),
          m_frame_interval(-1.f) {
        set_tick_rate(32.f);

        // TODO: Figure out what to do with these?
        m_renderer.set_camera(&m_camera);
        m_renderer.set_viewport(&m_viewport);

        try_invoke_callback(m_game.on_create, this);
    }

    game_engine::~game_engine() {
        try_invoke_callback(m_game.on_destroy, this);
    }

    void game_engine::run() {
        Uint64 last_frame_start_time = SDL_GetPerformanceCounter();
        const Uint64 performance_frequency = SDL_GetPerformanceFrequency();

        float time_since_last_tick = 0.f;

        while (m_is_running == true) {
            const Uint64 frame_start_time = SDL_GetPerformanceCounter();
            m_frame_interval = (frame_start_time - last_frame_start_time) /
                               static_cast<float>(performance_frequency);
            time_since_last_tick += m_frame_interval;
            last_frame_start_time = frame_start_time;

            process_events();

            while (time_since_last_tick >= m_tick_interval) {
                try_invoke_callback(m_game.on_tick, this, m_tick_interval);
                time_since_last_tick -= m_tick_interval;
            }

            m_fraction_to_next_tick = time_since_last_tick / m_tick_interval;

            try_invoke_callback(m_game.on_frame, this, m_frame_interval);

            m_renderer.frame_begin();
            try_invoke_callback(m_game.on_draw, this, m_fraction_to_next_tick);
            m_renderer.frame_end();
        }
    }

    void game_engine::process_events() {
        m_input.update();

        SDL_Event event;
        while (SDL_PollEvent(&event) == true) {
            if (event.type == SDL_EVENT_QUIT) {
                m_is_running = false;
            }

            m_input.process_event(event);
        }
    }
}  // namespace engine
