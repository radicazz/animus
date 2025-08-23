#include "engine.hxx"

namespace engine {
    game_engine::game_engine(const game_details& details, void* state,
                             const game_callbacks& callbacks)
        : m_window(details.title, static_cast<int>(details.size.x),
                   static_cast<int>(details.size.y)),
          m_renderer(m_window.get_sdl_window()),
          m_camera({0.f, 0.f}, 1.f),
          m_resource_manager(m_renderer.get_sdl_renderer()),
          m_input(),
          m_state(state),
          m_callbacks(callbacks) {
    }

    game_engine::~game_engine() {
    }

    void game_engine::run() {
        m_callbacks.create(this);

        // Timing variables for calculating delta time.
        Uint64 last_time = SDL_GetPerformanceCounter();
        const Uint64 frequency = SDL_GetPerformanceFrequency();

        while (m_window.is_running() == true) {
            m_input.update();

            Uint64 current_time = SDL_GetPerformanceCounter();
            const float delta_time = (current_time - last_time) / static_cast<float>(frequency);
            last_time = current_time;

            SDL_Event event;
            while (SDL_PollEvent(&event) == true) {
                if (event.type == SDL_EVENT_QUIT) {
                    m_window.set_is_running(false);
                }

                m_input.process_event(event);
            }

            m_callbacks.update(this, delta_time);

            m_renderer.frame_begin();
            m_callbacks.render(this);
            m_renderer.frame_end();
        }

        m_callbacks.destroy(this);
    }
}  // namespace engine
