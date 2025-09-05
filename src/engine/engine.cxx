#include "engine.hxx"

#include <stdexcept>

namespace engine {
    game_engine::game_engine(const game_details& details, void* state,
                             const game_callbacks& callbacks)
        : m_window(details.window_title, static_cast<int>(details.window_size.x),
                   static_cast<int>(details.window_size.y)),
          m_renderer(m_window.get_sdl_window()),
          m_camera({0.f, 0.f}, 1.f),
          m_resource_manager(m_renderer.get_sdl_renderer()),
          m_input_system(),
          m_state(state),
          m_callbacks(callbacks) {
        if (m_state == nullptr) {
            throw std::invalid_argument("Game state is a required engine parameter.");
        }

        // Set up camera with proper viewport size
        m_camera.set_viewport_size(details.window_size);
        
        // Set default world bounds for the camera
        // These can be overridden by calling engine.get_camera().set_bounds() if needed
        m_camera.set_bounds({-500.0f, -500.0f}, {1500.0f, 1500.0f});
        
        // Connect camera to renderer
        m_renderer.set_camera(&m_camera);

        if (m_callbacks.on_create != nullptr) {
            m_callbacks.on_create(this);
        }
    }

    game_engine::~game_engine() {
        if (m_callbacks.on_destroy != nullptr) {
            m_callbacks.on_destroy(this);
        }
    }

    void game_engine::run() {
        // Timing variables for calculating delta time.
        Uint64 last_frame_start_time = SDL_GetPerformanceCounter();
        const Uint64 performance_frequency = SDL_GetPerformanceFrequency();

        while (m_window.is_running() == true) {
            const Uint64 frame_start_time = SDL_GetPerformanceCounter();
            const float delta_time = (frame_start_time - last_frame_start_time) /
                                     static_cast<float>(performance_frequency);
            last_frame_start_time = frame_start_time;

            m_input_system.update();

            SDL_Event event;
            while (SDL_PollEvent(&event) == true) {
                if (event.type == SDL_EVENT_QUIT) {
                    m_window.set_is_running(false);
                }

                m_input_system.process_event(event);
            }

            if (m_callbacks.on_update != nullptr) {
                m_callbacks.on_update(this, delta_time);
            }

            m_renderer.frame_begin();
            if (m_callbacks.on_render != nullptr) {
                m_callbacks.on_render(this);
            }
            m_renderer.frame_end();
        }
    }
}  // namespace engine
