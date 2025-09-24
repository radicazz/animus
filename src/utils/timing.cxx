#include "timing.hxx"

#include <SDL3/SDL.h>

namespace engine {
    std::uint64_t performance_counter_value_current() noexcept {
        return static_cast<std::uint64_t>(SDL_GetPerformanceCounter());
    }

    float performance_counter_seconds_between(const std::uint64_t counter_start,
                                           const std::uint64_t counter_end) noexcept {
        const std::uint64_t frequency = SDL_GetPerformanceFrequency();
        return static_cast<float>(counter_end - counter_start) / static_cast<float>(frequency);
    }
}  // namespace engine
