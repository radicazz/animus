#pragma once

#include <type_traits>
#include <stdexcept>
#include <format>
#include <source_location>
#include <functional>
#include <optional>

#include "config.hxx"

namespace engine {
    class error_message : public std::runtime_error {
    public:
        template <class... Args>
        explicit error_message(std::string_view fmt, Args... args)
            : std::runtime_error(std::vformat(fmt, std::make_format_args(args...))) {
        }
    };

    /**
     * @brief Ensure a condition is true, throw if not (paranoid builds only).
     * @param condition The condition to check.
     * @param msg The message to include in the exception if the condition is false.
     */
    inline void ensure(const bool [[maybe_unused]] condition, std::string_view [[maybe_unused]] msg,
                       const std::source_location loc = std::source_location::current()) {
        if constexpr (is_paranoid_build) {
            if (!condition) {
                throw error_message("{}:{} [{}] -> {}", loc.file_name(), loc.line(),
                                    loc.function_name(), msg);
            }
        }
    }

    template <class F, class... Args>
        requires std::invocable<F, Args...>
    auto safe_invoke(F&& fn, Args&&... args)
        -> std::conditional_t<std::is_void_v<std::invoke_result_t<F, Args...>>, void,
                              std::optional<std::invoke_result_t<F, Args...>>> {
        using return_type = std::invoke_result_t<F, Args...>;
        using function_type = std::remove_cvref_t<std::remove_reference_t<F>>;

        if constexpr (std::is_pointer_v<function_type> || std::is_member_pointer_v<function_type>) {
            if (fn == nullptr) {
                if constexpr (std::is_void_v<return_type>) {
                    return;
                } else {
                    return std::optional<return_type>{};
                }
            }
        }

        if constexpr (std::is_void_v<return_type>) {
            std::invoke(std::forward<F>(fn), std::forward<Args>(args)...);
        } else {
            return std::optional<return_type>{
                std::invoke(std::forward<F>(fn), std::forward<Args>(args)...)};
        }
    }
}  // namespace engine
