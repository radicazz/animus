/**
 * @file logger.hxx
 * @brief Logging utilities for the game engine.
 *
 * This file provides various wrappers around SDL's logging functions with two major featuress
 * added.
 *
 * First, these functions use C++20's `std::format` for type-safe, modern string formatting
 * instead of SDL's C-style `printf` formatting.
 *
 * Second, depending on your CMake configuration, logging at different levels can be completely
 * avoided from the binary to eliminate any runtime overhead when logging is not needed.
 */

#pragma once

#include <format>
#include <SDL3/SDL.h>
#include "config.hxx"

namespace engine {
    /**
     * @brief Log a formatted message to the standard output with SDL_Log.
     * @param fmt The format string.
     * @param args The arguments to format into the string.
     * @note Prefer using `log_info`, `log_warning`, or `log_error` for level-specific logging.
     */
    template <class... Args>
    void log_formatted(std::string_view fmt, Args... args) {
        if constexpr (should_log_info || should_log_warnings || should_log_errors) {
            std::string message = std::vformat(fmt, std::make_format_args(args...));
            SDL_Log("%s", message.c_str());
        }
    }

    /**
     * @brief Log an informational message.
     * @param fmt The format string.
     * @param args The arguments to format into the string.
     * @note Only compiled if `ENGINE_LOG_INFO` is ON.
     */
    template <class... Args>
    void log_info(std::string_view fmt, Args... args) {
        if constexpr (should_log_info) {
            log_formatted(fmt, args...);
        }
    }

    /**
     * @brief Log a warning message.
     * @param fmt The format string.
     * @param args The arguments to format into the string.
     * @note Only compiled if `ENGINE_LOG_WARNINGS` is ON.
     */
    template <class... Args>
    void log_warning(std::string_view fmt, Args... args) {
        if constexpr (should_log_warnings) {
            log_formatted(fmt, args...);
        }
    }

    /**
     * @brief Log an error message.
     * @param fmt The format string.
     * @param args The arguments to format into the string.
     * @note Only compiled if `ENGINE_LOG_ERRORS` is ON.
     */
    template <class... Args>
    void log_error(std::string_view fmt, Args... args) {
        if constexpr (should_log_errors) {
            log_formatted(fmt, args...);
        }
    }
}  // namespace engine
