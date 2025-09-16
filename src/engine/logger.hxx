#pragma once

#include "config.hxx"

#include <format>
#include <SDL3/SDL.h>

namespace engine {
    template <class... Args>
    void log_formatted(std::string_view fmt, Args... args) {
        if constexpr (should_log_info || should_log_warnings || should_log_errors) {
            std::string message = std::vformat(fmt, std::make_format_args(args...));
            SDL_Log("%s", message.c_str());
        }
    }

    template <class... Args>
    void log_info(std::string_view fmt, Args... args) {
        if constexpr (should_log_info) {
            log_formatted(fmt, args...);
        }
    }

    template <class... Args>
    void log_warning(std::string_view fmt, Args... args) {
        if constexpr (should_log_warnings) {
            log_formatted(fmt, args...);
        }
    }

    template <class... Args>
    void log_error(std::string_view fmt, Args... args) {
        if constexpr (should_log_errors) {
            log_formatted(fmt, args...);
        }
    }
}  // namespace engine
