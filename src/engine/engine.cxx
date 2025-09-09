#include "engine.hxx"

#include <stdexcept>

namespace engine {
    template <class... Args>
    void safe_invoke(void (*func)(Args...), Args... args) {
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
          m_info(info),
          m_is_running(true),
          m_interpolation_alpha(0.0f) {
        // TODO: Figure out what to do with these?
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
        float time_accumulator_seconds = 0.0f;

        while (m_is_running == true) {
            const Uint64 frame_start_time = SDL_GetPerformanceCounter();
            const float delta_time = (frame_start_time - last_frame_start_time) /
                                     static_cast<float>(performance_frequency);
            time_accumulator_seconds += delta_time;
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
            while (time_accumulator_seconds >= fixed_timestep_seconds) {
                safe_invoke(m_info.on_fixed_update, this, fixed_timestep_seconds);
                time_accumulator_seconds -= fixed_timestep_seconds;
            }

            m_interpolation_alpha = time_accumulator_seconds / fixed_timestep_seconds;

            safe_invoke(m_info.on_update, this, delta_time);

            m_renderer.frame_begin();
            safe_invoke(m_info.on_render, this, m_interpolation_alpha);
            m_renderer.frame_end();
        }
    }
}  // namespace engine
