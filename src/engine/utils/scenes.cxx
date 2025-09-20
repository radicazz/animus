/**
 * @file scenes.cxx
 * @brief Scene management system implementation.
 */

#include "scenes.hxx"

#include "../engine.hxx"
#include "../logger.hxx"

#include <stdexcept>

namespace engine {
    game_scenes::game_scenes(game_engine* engine) : m_engine(engine) {
        if (!engine) {
            throw std::invalid_argument("game_engine pointer cannot be null");
        }
    }

    game_scenes::~game_scenes() {
        // Clean up all loaded scenes
        for (auto& [scene_id, scene_info] : m_scenes) {
            if (scene_info->state != scene_state::unloaded) {
                cleanup_scene_resources(scene_info.get());
            }
        }
        m_scenes.clear();
    }

    void game_scenes::register_scene(std::string_view scene_id, const game_scene& scene) {
        std::string id_str{scene_id};

        if (has_scene(id_str)) {
            log_warning("Scene '{}' is already registered. Overriding existing registration.",
                        id_str);
        }

        auto scene_info = std::make_unique<game_scene_info>();
        scene_info->scene_id = id_str;
        scene_info->state = scene_state::unloaded;
        scene_info->scene_state = nullptr;
        scene_info->scene = const_cast<game_scene*>(&scene);
        scene_info->engine = m_engine;

        m_scenes[id_str] = std::move(scene_info);
        log_info("Registered scene '{}'", id_str);
    }

    void game_scenes::unregister_scene(std::string_view scene_id) {
        std::string id_str{scene_id};

        auto it = m_scenes.find(id_str);
        if (it == m_scenes.end()) {
            log_warning("Attempted to unregister non-existent scene '{}'", id_str);
            return;
        }

        // If this is the active scene, deactivate it first
        if (m_active_scene_id == id_str) {
            deactivate_current_scene();
        }

        // Clean up resources if the scene is loaded
        if (it->second->state != scene_state::unloaded) {
            cleanup_scene_resources(it->second.get());
        }

        m_scenes.erase(it);
        log_info("Unregistered scene '{}'", id_str);
    }

    bool game_scenes::has_scene(std::string_view scene_id) const {
        return m_scenes.find(std::string{scene_id}) != m_scenes.end();
    }

    bool game_scenes::has_active_scene() const {
        return !m_active_scene_id.empty();
    }

    const std::string& game_scenes::get_active_scene_id() const {
        return m_active_scene_id;
    }

    void game_scenes::load_scene(std::string_view scene_id, void* scene_state) {
        std::string id_str{scene_id};

        auto it = m_scenes.find(id_str);
        if (it == m_scenes.end()) {
            throw std::runtime_error("Scene '" + id_str + "' is not registered");
        }

        auto& scene_info = it->second;

        if (scene_info->state != scene_state::unloaded) {
            log_warning("Scene '{}' is already loaded (state: {})", id_str,
                        static_cast<int>(scene_info->state));
            return;
        }

        log_info("Loading scene '{}'", id_str);
        scene_info->state = scene_state::loading;
        scene_info->scene_state = scene_state;

        try {
            // Initialize scene systems
            scene_info->entities = std::make_unique<game_entities>();
            scene_info->resources = std::make_unique<game_resources>(m_engine->get_renderer());

            // Create default camera and viewport
            create_default_camera_viewport(scene_info.get());

            // Call scene load callback
            if (scene_info->scene->on_load) {
                scene_info->scene->on_load(scene_info.get());
            }

            // Scene is now loaded but not active yet
            scene_info->state = scene_state::paused;
            log_info("Scene '{}' loaded successfully", id_str);
        } catch (const std::exception& e) {
            log_error("Failed to load scene '{}': {}", id_str, e.what());
            // Clean up partially loaded resources
            cleanup_scene_resources(scene_info.get());
            scene_info->state = scene_state::unloaded;
            throw;
        }
    }

    void game_scenes::activate_scene(std::string_view scene_id, scene_transition transition) {
        std::string id_str{scene_id};

        auto it = m_scenes.find(id_str);
        if (it == m_scenes.end()) {
            throw std::runtime_error("Scene '" + id_str + "' is not registered");
        }

        auto& scene_info = it->second;

        if (scene_info->state == scene_state::unloaded) {
            throw std::runtime_error("Scene '" + id_str + "' must be loaded before activation");
        }

        if (scene_info->state == scene_state::active) {
            log_warning("Scene '{}' is already active", id_str);
            return;
        }

        // Deactivate current scene if one is active
        if (has_active_scene()) {
            deactivate_current_scene_with_transition(transition);
        }

        log_info("Activating scene '{}'", id_str);

        // Call transition in callback for new scene
        if (scene_info->scene->on_transition_in) {
            scene_info->scene->on_transition_in(scene_info.get(), transition);
        }

        // Call scene activate callback
        if (scene_info->scene->on_activate) {
            scene_info->scene->on_activate(scene_info.get());
        }

        scene_info->state = scene_state::active;
        m_active_scene_id = id_str;

        log_info("Scene '{}' activated successfully", id_str);
    }

    void game_scenes::deactivate_current_scene_with_transition(scene_transition transition) {
        if (!has_active_scene()) {
            log_warning("No active scene to deactivate");
            return;
        }

        auto it = m_scenes.find(m_active_scene_id);
        if (it == m_scenes.end()) {
            log_error("Active scene '{}' not found in scene registry", m_active_scene_id);
            m_active_scene_id.clear();
            return;
        }

        auto& scene_info = it->second;

        // Validate scene is in active state
        if (scene_info->state != scene_state::active) {
            log_error("Scene '{}' is not in active state (current: {}), cannot deactivate",
                      m_active_scene_id, static_cast<int>(scene_info->state));
            return;
        }

        log_info("Deactivating scene '{}' with transition", m_active_scene_id);

        try {
            // Call scene transition out callback
            if (scene_info->scene->on_transition_out) {
                scene_info->scene->on_transition_out(scene_info.get(), transition);
            }

            // Call scene deactivate callback
            if (scene_info->scene->on_deactivate) {
                scene_info->scene->on_deactivate(scene_info.get());
            }

            scene_info->state = scene_state::paused;
            m_active_scene_id.clear();

            log_info("Scene deactivated successfully");
        } catch (const std::exception& e) {
            log_error("Exception during scene deactivation: {}", e.what());
            // Force state transition to prevent inconsistent state
            scene_info->state = scene_state::paused;
            m_active_scene_id.clear();
            throw;
        }
    }

    void game_scenes::deactivate_current_scene() {
        if (!has_active_scene()) {
            log_warning("No active scene to deactivate");
            return;
        }

        auto it = m_scenes.find(m_active_scene_id);
        if (it == m_scenes.end()) {
            log_error("Active scene '{}' not found in scene registry", m_active_scene_id);
            m_active_scene_id.clear();
            return;
        }

        auto& scene_info = it->second;

        // Validate scene is in active state
        if (scene_info->state != scene_state::active) {
            log_error("Scene '{}' is not in active state (current: {}), cannot deactivate",
                      m_active_scene_id, static_cast<int>(scene_info->state));
            return;
        }

        log_info("Deactivating scene '{}'", m_active_scene_id);

        try {
            // Call scene deactivate callback
            if (scene_info->scene->on_deactivate) {
                scene_info->scene->on_deactivate(scene_info.get());
            }

            scene_info->state = scene_state::paused;
            m_active_scene_id.clear();

            log_info("Scene deactivated successfully");
        } catch (const std::exception& e) {
            log_error("Exception during scene deactivation: {}", e.what());
            // Force state transition to prevent inconsistent state
            scene_info->state = scene_state::paused;
            m_active_scene_id.clear();
            throw;
        }
    }

    void game_scenes::unload_scene(std::string_view scene_id) {
        std::string id_str{scene_id};

        auto it = m_scenes.find(id_str);
        if (it == m_scenes.end()) {
            log_warning("Attempted to unload non-existent scene '{}'", id_str);
            return;
        }

        auto& scene_info = it->second;

        if (scene_info->state == scene_state::unloaded) {
            log_warning("Scene '{}' is already unloaded", id_str);
            return;
        }

        // If this is the active scene, deactivate it first
        if (m_active_scene_id == id_str) {
            deactivate_current_scene();
        }

        log_info("Unloading scene '{}'", id_str);
        scene_info->state = scene_state::unloading;

        try {
            // Call scene unload callback
            if (scene_info->scene->on_unload) {
                scene_info->scene->on_unload(scene_info.get());
            }

            // Clean up scene resources
            cleanup_scene_resources(scene_info.get());

            scene_info->state = scene_state::unloaded;
            log_info("Scene '{}' unloaded successfully", id_str);
        } catch (const std::exception& e) {
            log_error("Exception during scene unloading: {}", e.what());
            // Force unloaded state even on failure to prevent resource leaks
            scene_info->state = scene_state::unloaded;

            // Clean up resources anyway to prevent leaks
            try {
                cleanup_scene_resources(scene_info.get());
            } catch (const std::exception& cleanup_e) {
                log_error("Exception during emergency scene cleanup: {}", cleanup_e.what());
            }

            throw;
        }
    }

    void game_scenes::switch_to_scene(std::string_view scene_id, void* scene_state,
                                      scene_transition transition) {
        load_scene(scene_id, scene_state);
        activate_scene(scene_id, transition);
    }

    game_scene_info* game_scenes::get_active_scene() {
        if (!has_active_scene()) {
            return nullptr;
        }

        auto it = m_scenes.find(m_active_scene_id);
        if (it == m_scenes.end()) {
            log_error("Active scene '{}' not found in scene registry", m_active_scene_id);
            m_active_scene_id.clear();
            return nullptr;
        }

        return it->second.get();
    }

    const game_scene_info* game_scenes::get_active_scene() const {
        if (!has_active_scene()) {
            return nullptr;
        }

        auto it = m_scenes.find(m_active_scene_id);
        if (it == m_scenes.end()) {
            log_error("Active scene '{}' not found in scene registry", m_active_scene_id);
            return nullptr;
        }

        return it->second.get();
    }

    game_camera& game_scenes::get_camera(std::string_view name) {
        auto* active_scene = get_active_scene();
        if (!active_scene) {
            throw std::runtime_error("No active scene available for camera access");
        }

        std::string name_str{name};
        auto it = active_scene->cameras.find(name_str);
        if (it == active_scene->cameras.end()) {
            throw std::runtime_error("Camera '" + name_str + "' not found in active scene");
        }

        return *it->second;
    }

    game_viewport& game_scenes::get_viewport(std::string_view name) {
        auto* active_scene = get_active_scene();
        if (!active_scene) {
            throw std::runtime_error("No active scene available for viewport access");
        }

        std::string name_str{name};
        auto it = active_scene->viewports.find(name_str);
        if (it == active_scene->viewports.end()) {
            throw std::runtime_error("Viewport '" + name_str + "' not found in active scene");
        }

        return *it->second;
    }

    void game_scenes::for_each_scene(void (*callback)(const std::string&,
                                                      const game_scene_info&)) const {
        if (!callback) {
            return;
        }

        for (const auto& [scene_id, scene_info] : m_scenes) {
            callback(scene_id, *scene_info);
        }
    }

    void game_scenes::create_default_camera_viewport(game_scene_info* scene_info) {
        // Create default main camera with default position (0,0) and zoom (1.0)
        std::string main_camera{game_scene_info::default_camera_name};
        scene_info->cameras[main_camera] =
            std::make_unique<game_camera>(glm::vec2{0.0f, 0.0f}, 1.0f);

        // Create default main viewport
        std::string main_viewport{game_scene_info::default_viewport_name};
        scene_info->viewports[main_viewport] = std::make_unique<game_viewport>();
    }

    void game_scenes::cleanup_scene_resources(game_scene_info* scene_info) {
        if (!scene_info) {
            return;
        }

        // Clear all cameras
        scene_info->cameras.clear();

        // Clear all viewports
        scene_info->viewports.clear();

        // Reset scene systems (RAII will handle cleanup)
        scene_info->entities.reset();
        scene_info->resources.reset();

        // Clear scene state reference
        scene_info->scene_state = nullptr;
    }
}  // namespace engine
