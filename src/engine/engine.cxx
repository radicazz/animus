#include "engine.hxx"

#include <stdexcept>

namespace engine {
    game_engine::game_engine(const game_details& details, const game_info& info)
        : m_window(details.window_title, static_cast<int>(details.window_size.x),
                   static_cast<int>(details.window_size.y)),
          m_renderer(m_window.get_sdl_window()),
          m_camera({0.f, 0.f}, 1.f),
          m_viewport(details.window_size),
          m_resources(m_renderer),
          m_input(),
          m_ecs_manager(),
          m_info(info),
          m_is_running(true) {
        m_renderer.set_camera(&m_camera);
        m_renderer.set_viewport(&m_viewport);

        safe_invoke(m_info.on_create, this);
    }

    game_engine::~game_engine() {
        safe_invoke(m_info.on_destroy, this);
    }

    void game_engine::run() {
        Uint64 last_frame_start_time = SDL_GetPerformanceCounter();
        const Uint64 performance_frequency = SDL_GetPerformanceFrequency();

        constexpr float fixed_timestep_seconds = 1.0f / 32.0f;
        float fixed_time_accumulator = 0.0f;

        while (m_is_running == true) {
            const Uint64 frame_start_time = SDL_GetPerformanceCounter();
            const float delta_time = (frame_start_time - last_frame_start_time) /
                                     static_cast<float>(performance_frequency);
            fixed_time_accumulator += delta_time;
            last_frame_start_time = frame_start_time;

            m_input.update();

            SDL_Event event;
            while (SDL_PollEvent(&event) == true) {
                if (event.type == SDL_EVENT_QUIT) {
                    m_is_running = false;
                }

                m_input.process_event(event);
            }

            // Run fixed update as many times as needed to catch up
            while (fixed_time_accumulator >= fixed_timestep_seconds) {
                safe_invoke(m_info.on_fixed_update, this, fixed_timestep_seconds);
                fixed_time_accumulator -= fixed_timestep_seconds;
            }

            const float interpolation_alpha = fixed_time_accumulator / fixed_timestep_seconds;

            safe_invoke(m_info.on_update, this, delta_time);

            m_renderer.frame_begin();
            safe_invoke(m_info.on_render, this, interpolation_alpha);
            m_renderer.frame_end();
        }
    }
}  // namespace engine
