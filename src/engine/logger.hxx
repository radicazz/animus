#pragma once

#include <SDL3/SDL.h>
#include <format>
#include <type_traits>

namespace engine {
    enum class log_level : unsigned int {
        info = 1 << 0,
        verbose = 1 << 1,
        warning = 1 << 2,
        error = 1 << 3
    };

    constexpr log_level operator|(log_level lhs, log_level rhs) {
        return static_cast<log_level>(static_cast<std::underlying_type_t<log_level>>(lhs) |
                                      static_cast<std::underlying_type_t<log_level>>(rhs));
    }

    constexpr log_level operator&(log_level lhs, log_level rhs) {
        return static_cast<log_level>(static_cast<std::underlying_type_t<log_level>>(lhs) &
                                      static_cast<std::underlying_type_t<log_level>>(rhs));
    }

    constexpr bool logging_enabled = true;
    constexpr auto logging_level = log_level::info | log_level::warning | log_level::error;

    consteval bool should_compile_log_level(log_level level) {
        if constexpr (logging_enabled == true) {
            return (logging_level & level) != static_cast<log_level>(0);
        }

        return false;
    }

    template <log_level Level, typename... Args>
    void game_log(std::string_view format_str, [[maybe_unused]] Args&&... args) {
        if constexpr (should_compile_log_level(Level) == true) {
            const std::string message = std::vformat(format_str, std::make_format_args(args...));
            switch (Level) {
                case log_level::info:
                    SDL_Log("[info] %s", message.c_str());
                    break;
                case log_level::verbose:
                    SDL_Log("[verbose] %s", message.c_str());
                    break;
                case log_level::warning:
                    SDL_Log("[warning] %s", message.c_str());
                    break;
                case log_level::error:
                    SDL_Log("[error] %s", message.c_str());
                    break;
                default:
                    SDL_Log("%s", message.c_str());
                    break;
            }
        }
    }
}  // namespace engine
