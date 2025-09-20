/**
 * @file scenes.hxx
 * @brief Scene management system for the game engine.
 *
 * This file defines the scene management system that enables modular, flexible scene switching
 * with per-scene resources, entities, cameras, and viewports while maintaining compatibility
 * with the existing engine architecture.
 */

#pragma once

#include <unordered_map>
#include <string>
#include <string_view>
#include <memory>
#include <type_traits>

#include "resources.hxx"
#include "../ecs/entities.hxx"
#include "../renderer/camera.hxx"
#include "../renderer/viewport.hxx"

namespace engine {
    // Forward declarations
    class game_engine;
    class game_scenes;
    struct game_scene;
    struct game_scene_info;
    class game_input;

    /**
     * @brief Scene state management enumeration.
     *
     * Represents the current lifecycle state of a scene within the engine.
     */
    enum class scene_state {
        unloaded,  ///< Scene is not loaded in memory
        loading,   ///< Scene is currently being loaded
        active,    ///< Scene is currently active and running
        paused,    ///< Scene is loaded but paused
        unloading  ///< Scene is currently being unloaded
    };

    /**
     * @brief Scene transition types for scene switching.
     *
     * Defines different visual transition effects that can be applied when switching scenes.
     */
    enum class scene_transition {
        immediate,  ///< Instant scene switch with no visual effect
        fade_in,    ///< Fade in from black
        fade_out,   ///< Fade out to black
        crossfade,  ///< Crossfade between scenes
        custom      ///< Custom transition defined by the game
    };

    /**
     * @brief Contains scene-specific data and systems.
     *
     * This structure holds all the data associated with a specific scene, including its state,
     * resources, entities, cameras, and viewports. It serves as the primary interface for
     * scene callbacks to access scene-specific functionality.
     */
    struct game_scene_info {
        // Scene identification
        std::string scene_id;
        scene_state state = scene_state::unloaded;

        // Scene-specific state (similar to current game_info::state)
        void* scene_state = nullptr;

        // Back-references for scene callbacks
        game_scene* scene = nullptr;
        game_engine* engine = nullptr;

        // Scene-specific systems
        std::unique_ptr<game_entities> entities;
        std::unique_ptr<game_resources> resources;

        // Multi-camera/viewport support
        std::unordered_map<std::string, std::unique_ptr<game_camera>> cameras;
        std::unordered_map<std::string, std::unique_ptr<game_viewport>> viewports;

        // Default camera/viewport names
        static constexpr std::string_view default_camera_name = "main";
        static constexpr std::string_view default_viewport_name = "main";

        /**
         * @brief Get the scene-specific state as the specified type.
         * @tparam T The type to cast the scene state to. Must be a class type.
         * @return Reference to the scene state as type T.
         */
        template <class T>
            requires std::is_class_v<T>
        [[nodiscard]] T& get_state();
    };

    /**
     * @brief Scene callback function definitions.
     *
     * This structure contains function pointers for all scene lifecycle and frame callbacks.
     * Games implement these callbacks to define scene-specific behavior.
     */
    struct game_scene {
        void (*on_load)(game_scene_info* scene) = nullptr;
        void (*on_unload)(game_scene_info* scene) = nullptr;
        void (*on_activate)(game_scene_info* scene) = nullptr;
        void (*on_deactivate)(game_scene_info* scene) = nullptr;

        void (*on_tick)(game_scene_info* scene, float tick_interval) = nullptr;
        void (*on_frame)(game_scene_info* scene, float frame_interval) = nullptr;
        void (*on_draw)(game_scene_info* scene, float fraction_to_next_tick) = nullptr;

        void (*on_input)(game_scene_info* scene, const game_input& input) = nullptr;

        void (*on_transition_in)(game_scene_info* scene, scene_transition transition) = nullptr;
        void (*on_transition_out)(game_scene_info* scene, scene_transition transition) = nullptr;
    };

    /**
     * @brief Scene management system for the game engine.
     *
     * This class manages the registration, loading, activation, and cleanup of game scenes.
     * It provides a complete scene system with per-scene resources, entities, cameras,
     * and viewports while maintaining integration with the existing engine architecture.
     */
    class game_scenes {
    public:
        game_scenes() = delete;
        explicit game_scenes(game_engine* engine);
        ~game_scenes();

        game_scenes(const game_scenes&) = delete;
        game_scenes& operator=(const game_scenes&) = delete;
        game_scenes(game_scenes&&) = delete;
        game_scenes& operator=(game_scenes&&) = delete;

        void register_scene(std::string_view scene_id, const game_scene& scene);
        void unregister_scene(std::string_view scene_id);

        [[nodiscard]] bool has_scene(std::string_view scene_id) const;
        [[nodiscard]] bool has_active_scene() const;
        [[nodiscard]] const std::string& get_active_scene_id() const;

        void load_scene(std::string_view scene_id, void* scene_state = nullptr);
        void activate_scene(std::string_view scene_id,
                            scene_transition transition = scene_transition::immediate);
        void deactivate_current_scene();
        void unload_scene(std::string_view scene_id);

        void switch_to_scene(std::string_view scene_id, void* scene_state = nullptr,
                             scene_transition transition = scene_transition::immediate);

        [[nodiscard]] game_scene_info* get_active_scene();
        [[nodiscard]] const game_scene_info* get_active_scene() const;

        [[nodiscard]] game_camera& get_camera(std::string_view name = "main");
        [[nodiscard]] game_viewport& get_viewport(std::string_view name = "main");

        void for_each_scene(void (*callback)(const std::string&, const game_scene_info&)) const;

    private:
        void create_default_camera_viewport(game_scene_info* scene_info);
        void cleanup_scene_resources(game_scene_info* scene_info);
        void deactivate_current_scene_with_transition(scene_transition transition);

    private:
        game_engine* m_engine;
        std::unordered_map<std::string, std::unique_ptr<game_scene_info>> m_scenes;
        std::string m_active_scene_id;
    };

    template <class T>
        requires std::is_class_v<T>
    inline T& game_scene_info::get_state() {
        return *static_cast<T*>(scene_state);
    }

}  // namespace engine
