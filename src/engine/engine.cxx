#include "engine.hxx"

namespace engine {
    game_engine::game_engine(std::string_view title, glm::vec2 size, void* game_instance,
                             const game_callbacks& callbacks)
        : m_window(title, size.x, size.y),
          m_renderer(m_window.get_window()),
          m_game(game_instance),
          m_callbacks(callbacks) {
    }

    game_engine::~game_engine() {
    }

    void game_engine::run() {
        m_callbacks.create(this);

        while (m_window.is_running() == true) {
            SDL_Event event;
            while (SDL_PollEvent(&event) == true) {
                if (event.type == SDL_EVENT_QUIT) {
                    m_window.set_is_running(false);
                }

                m_callbacks.process_events(this, &event);
            }

            m_renderer.render_pre();
            m_callbacks.render(this);
            m_renderer.render_post();
        }

        m_callbacks.destroy(this);
    }
}  // namespace engine