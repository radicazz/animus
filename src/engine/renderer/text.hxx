#pragma once

#include <glm/glm.hpp>
#include <string>
#include <format>
#include "color.hxx"

struct TTF_Text;
struct TTF_Font;

namespace engine {
    /**
     * @brief Represents a text object in the game.
     */
    class render_text {
    public:
        render_text() = delete;
        render_text(TTF_Text* sdl_text);
        ~render_text();

        /**
         * @brief Get the underlying SDL text object.
         * @return Pointer to the SDL text object.
         * @note This utility should only be used internally by the game engine.
         */
        [[nodiscard]] TTF_Text* get_sdl_text() const;

        [[nodiscard]] render_color get_color() const;
        [[nodiscard]] glm::vec2 get_size() const;
        [[nodiscard]] glm::vec2 get_origin() const;

        void set_text_raw(std::string_view new_text);

        template <typename... Args>
        void set_text(std::format_string<Args...> fmt, Args&&... args);

        void set_color(const render_color& new_color);
        void set_origin(const glm::vec2& new_origin);

        [[nodiscard]] bool is_valid() const;

    private:
        TTF_Text* m_sdl_text;
        glm::vec2 m_origin;
    };

    inline TTF_Text* render_text::get_sdl_text() const {
        return m_sdl_text;
    }

    template <typename... Args>
    inline void render_text::set_text(std::format_string<Args...> fmt, Args&&... args) {
        std::string formatted_text = std::format(fmt, std::forward<Args>(args)...);
        set_text_raw(formatted_text);
    }

    inline bool render_text::is_valid() const {
        return m_sdl_text != nullptr;
    }

    inline glm::vec2 render_text::get_origin() const {
        return m_origin;
    }

    inline void render_text::set_origin(const glm::vec2& new_origin) {
        m_origin = new_origin;
    }
}  // namespace engine