#include "safety.hxx"

#include <SDL3/SDL.h>

namespace engine {
    bool message_box_info(std::string_view title, std::string_view message) {
        return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.data(), message.data(),
                                        nullptr);
    }

    bool message_box_error(std::string_view title, std::string_view message) {
        return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.data(), message.data(),
                                        nullptr);
    }
}  // namespace engine
