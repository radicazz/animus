#include <SDL3/SDL.h>

namespace engine {
    class renderer {
    public:
        renderer(SDL_Window* window);
        ~renderer();

        SDL_Renderer* get_sdl_renderer() const;

        // Begin/end frame
        void begin_frame();
        void end_frame();

    private:
        SDL_Renderer* m_sdl_renderer;
    };

    inline SDL_Renderer* renderer::get_sdl_renderer() const {
        return m_sdl_renderer;
    }

}  // namespace engine