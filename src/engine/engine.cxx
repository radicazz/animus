#include "engine.hxx"

#include <stdexcept>

namespace engine {
    template <class... Args>
    void try_invoke_callback(void (*func)(Args...), Args... args) {
        if (func != nullptr) {
            func(args...);
        }
    }

    game_engine::game_engine(const game_info& info, std::string_view title, const glm::ivec2& size)
        : m_window(title, size),
          m_renderer(m_window.get_sdl_window()),
          m_camera({0.f, 0.f}, 1.f),
          m_viewport(size),
          m_resources(m_renderer),
          m_input(),
          m_entities(),
          m_game(info),
          m_is_running(true),
          m_fixed_delta_time(-1.f),
          m_interpolation_alpha(0.f),
          m_delta_time(0.f) {
        set_tick_rate(32.f);

        // TODO: Figure out what to do with these?
        m_renderer.set_camera(&m_camera);
        m_renderer.set_viewport(&m_viewport);

        // Allow the game to initialize itself.
        try_invoke_callback(m_game.on_create, this);
    }

    game_engine::~game_engine() {
        try_invoke_callback(m_game.on_destroy, this);
    }

    void game_engine::run() {
        Uint64 last_frame_start_time = SDL_GetPerformanceCounter();
        const Uint64 performance_frequency = SDL_GetPerformanceFrequency();

        float fixed_update_time_accumulator = 0.f;

        while (m_is_running == true) {
            const Uint64 frame_start_time = SDL_GetPerformanceCounter();
            m_delta_time = (frame_start_time - last_frame_start_time) /
                           static_cast<float>(performance_frequency);
            fixed_update_time_accumulator += m_delta_time;
            last_frame_start_time = frame_start_time;

            process_events();

            // Run fixed update as many times as needed to catch up
            while (fixed_update_time_accumulator >= m_fixed_delta_time) {
                try_invoke_callback(m_game.on_fixed_update, this, m_fixed_delta_time);
                fixed_update_time_accumulator -= m_fixed_delta_time;
            }

            // Calculate interpolation alpha for rendering.
            m_interpolation_alpha = fixed_update_time_accumulator / m_fixed_delta_time;

            try_invoke_callback(m_game.on_update, this, m_delta_time);

            m_renderer.frame_begin();
            try_invoke_callback(m_game.on_render, this, m_interpolation_alpha);
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
