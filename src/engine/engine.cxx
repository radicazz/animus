#include "engine.hxx"

namespace engine {
    game_engine::game_engine(const game_details& details, void* state,
                             const game_callbacks& callbacks)
        : m_window(details.title, details.size.x, details.size.y),
          m_renderer(m_window.get_sdl_window()),
          m_camera({0.f, 0.f}, 1.f),
          m_state(state),
          m_callbacks(callbacks) {
    }

    game_engine::~game_engine() {
    }

    void game_engine::run() {
        // Run the game's startup code.
        m_callbacks.create(this);

        while (m_window.is_running() == true) {
            SDL_Event event;
            while (SDL_PollEvent(&event) == true) {
                if (event.type == SDL_EVENT_QUIT) {
                    // Close the window.
                    m_window.set_is_running(false);
                }

                // Pass events to game callback.
                m_callbacks.process_events(this, &event);
            }

            m_renderer.begin_frame();
            m_callbacks.render(this);
            m_renderer.end_frame();
        }

        m_callbacks.destroy(this);
    }
}  // namespace engine