#pragma once

#include <glm/glm.hpp>
#include <string>
#include <format>
#include "color.hxx"

struct TTF_Text;
struct TTF_Font;
struct SDL_Texture;

namespace engine {
    constexpr std::string_view default_text_content = "Text";

    /**
     * @brief Represents static text objects in the game.
     *
     * This class is very efficient and lightweight but does not support dynamic resizing,
     * scaling or rotating, making it unsuitable for some situations such as text placed
     * within the game world that needs to be scaled appropriately by the camera's zoom.
     *
     * Instead, this class should be used for UI elements, HUD labels, and other
     * text that does not need to change size during gameplay because it literally
     * cannot change size.
     *
     * @code
     * // Use the resource manager to create a static text object.
     * std::unique_ptr<render_text_static> text =
     * resource_manager.create_text_static("assets/fonts/arial.ttf", 24);
     * text->set_text("Hello, World!");
     *
     * // During rendering, use the game renderer to display your text.
     * game_renderer.draw_text_screen(text.get(), {10, 10});
     * @endcode
     */
    class render_text_static {
    public:
        render_text_static() = delete;
        render_text_static(TTF_Text* sdl_text);
        ~render_text_static();

        /**
         * @brief Access the internal SDL text object.
         * @return Pointer to the SDL text object.
         * @note This utility should only be used internally by game engine code.
         */
        [[nodiscard]] TTF_Text* get_sdl_text() const;

        [[nodiscard]] render_color get_color() const;
        [[nodiscard]] glm::vec2 get_size() const;
        [[nodiscard]] glm::vec2 get_origin() const;

        void set_text_raw(std::string_view new_text);

        /**
         * @brief Set the text content using std::format compatible formatting.
         * @tparam Args Variadic template arguments.
         * @param fmt The format string.
         * @param args The values to format.
         */
        template <typename... Args>
        void set_text(std::format_string<Args...> fmt, Args&&... args);

        void set_color(const render_color& new_color);
        void set_origin(const glm::vec2& new_origin);

        [[nodiscard]] bool is_valid() const;

    private:
        TTF_Text* m_sdl_text;
        glm::vec2 m_origin;
    };

    inline TTF_Text* render_text_static::get_sdl_text() const {
        return m_sdl_text;
    }

    template <typename... Args>
    inline void render_text_static::set_text(std::format_string<Args...> fmt, Args&&... args) {
        std::string formatted_text = std::format(fmt, std::forward<Args>(args)...);
        set_text_raw(formatted_text);
    }

    inline bool render_text_static::is_valid() const {
        return m_sdl_text != nullptr;
    }

    inline glm::vec2 render_text_static::get_origin() const {
        return m_origin;
    }

    inline void render_text_static::set_origin(const glm::vec2& new_origin) {
        m_origin = new_origin;
    }

    /**
     * @brief Represents dynamic text objects in the game.
     *
     * This class wraps the `render_text_static` class and adds support for dynamic text
     * resizing and scaling that can change during gameplay by using surfaces and textures - making
     * it ideal for game world text.
     *
     * @note Keep in mind that every time you change the text content or the color of this object,
     * the internal texture will need to be regenerated to reflect those changes, coming with a
     * slight performance cost.
     *
     * @code
     * // Use the resource manager to create a dynamic text object.
     * // Same interface as `render_text_static`.
     * std::unique_ptr<render_text_dynamic> dynamic_text =
     * resource_manager.create_dynamic_text("assets/fonts/arial.ttf", 32.f);
     *
     * // Triggers a texture regeneration next frame.
     * dynamic_text->set_text("Hello, Dynamic World!");
     * @endcode
     */
    class render_text_dynamic {
    public:
        render_text_dynamic(std::string_view content,
                            std::unique_ptr<render_text_static> static_text,
                            SDL_Renderer* sdl_renderer, TTF_Font* font);
        ~render_text_dynamic();

        render_text_dynamic(const render_text_dynamic&) = delete;
        render_text_dynamic& operator=(const render_text_dynamic&) = delete;
        render_text_dynamic(render_text_dynamic&&) = delete;
        render_text_dynamic& operator=(render_text_dynamic&&) = delete;

        /**
         * @brief Access the internal SDL texture for rendering the text.
         *
         * Changes to the internal text content or text color will set a flag indicating
         * that the texture needs to be regenerated next time this method is called.
         *
         * @return Pointer to the SDL texture.
         * @note This utility should only be used internally by the game renderer.
         */
        [[nodiscard]] SDL_Texture* get_sdl_texture() const;
        [[nodiscard]] const render_text_static* get_static_text() const;

        [[nodiscard]] render_color get_color() const;
        [[nodiscard]] glm::vec2 get_size() const;
        [[nodiscard]] glm::vec2 get_transformed_size() const;
        [[nodiscard]] glm::vec2 get_origin() const;
        [[nodiscard]] glm::vec2 get_scale() const;
        [[nodiscard]] float get_rotation() const;

        /**
         * @brief Set the raw text content.
         * @param new_text The new text content.
         * @note Changing the text content will mark the internal texture as dirty, requiring
         * regeneration.
         */
        void set_text_raw(std::string_view new_text);

        /**
         * @brief Set the text content using std::format compatible formatting.
         * @tparam Args Variadic template arguments.
         * @param fmt The format string.
         * @param args The values to format.
         * @note Changing the text content will mark the internal texture as dirty, requiring
         * regeneration.
         */
        template <typename... Args>
        void set_text(std::format_string<Args...> fmt, Args&&... args);

        /**
         * @brief Set the color of the text.
         * @param new_color The new color to set.
         * @note Changing the color will mark the internal texture as dirty, requiring regeneration.
         */
        void set_color(const render_color& new_color);
        void set_scale(const glm::vec2& new_scale);
        void set_scale(float uniform_scale);
        void set_rotation(float degrees);

        void set_origin(const glm::vec2& new_origin);
        void set_origin_centered();

        [[nodiscard]] bool is_valid() const;

    private:
        void mark_texture_dirty();
        void regenerate_texture_if_needed();
        SDL_Texture* create_texture_from_surface();

    private:
        std::unique_ptr<render_text_static> m_static_text;

        SDL_Renderer* m_sdl_renderer;
        TTF_Font* m_sdl_font;

        SDL_Texture* m_cached_texture;
        bool m_is_texture_dirty;

        std::string m_text_content;

        glm::vec2 m_scale;
        float m_rotation_degrees;
    };

    template <typename... Args>
    inline void render_text_dynamic::set_text(std::format_string<Args...> fmt, Args&&... args) {
        std::string formatted_text = std::format(fmt, std::forward<Args>(args)...);
        set_text_raw(formatted_text);
    }

    inline const render_text_static* render_text_dynamic::get_static_text() const {
        return m_static_text.get();
    }

    inline render_color render_text_dynamic::get_color() const {
        return m_static_text->get_color();
    }

    inline glm::vec2 render_text_dynamic::get_size() const {
        return m_static_text->get_size();
    }

    inline glm::vec2 render_text_dynamic::get_transformed_size() const {
        return get_size() * m_scale;
    }

    inline glm::vec2 render_text_dynamic::get_origin() const {
        return m_static_text->get_origin();
    }

    inline glm::vec2 render_text_dynamic::get_scale() const {
        return m_scale;
    }

    inline float render_text_dynamic::get_rotation() const {
        return m_rotation_degrees;
    }

    inline void render_text_dynamic::set_scale(const glm::vec2& new_scale) {
        m_scale = new_scale;
    }

    inline void render_text_dynamic::set_scale(float uniform_scale) {
        set_scale(glm::vec2{uniform_scale, uniform_scale});
    }

    inline void render_text_dynamic::set_rotation(float degrees) {
        m_rotation_degrees = degrees;
    }

    inline bool render_text_dynamic::is_valid() const {
        return m_static_text != nullptr && m_static_text->is_valid() == true &&
               m_sdl_renderer != nullptr && m_sdl_font != nullptr;
    }

    inline void render_text_dynamic::set_origin(const glm::vec2& new_origin) {
        m_static_text->set_origin(new_origin);
    }

    inline void render_text_dynamic::set_origin_centered() {
        const glm::vec2 size = get_size();
        set_origin(size * 0.5f);
    }

    inline void render_text_dynamic::mark_texture_dirty() {
        m_is_texture_dirty = true;
    }
}  // namespace engine
