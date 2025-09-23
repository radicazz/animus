/**
 * @file scenes.cxx
 * @brief Scene management system implementation.
 */

#include "scenes.hxx"

#include "../safety.hxx"
#include "../engine.hxx"
#include "../logger.hxx"

#include <stdexcept>

namespace engine {
    game_scenes::game_scenes(game_engine* engine) : m_engine(engine) {
        ensure(m_engine != nullptr, "game_engine pointer cannot be null");
    }

    game_scenes::~game_scenes() {
        for (auto& [scene_id, scene_info] : m_scenes) {
            if (scene_info->state != game_scene_lifetime::unloaded) {
                log_info("Destroyed scene '{}' during cleanup", scene_id);
                cleanup_scene_resources(scene_info.get());
            }
        }

        m_scenes.clear();
    }

    void game_scenes::register_scene(std::string_view scene_id, const game_scene_callbacks& scene) {
        auto id_str = std::string{scene_id};

        if (has_scene(id_str)) {
            log_warning("Scene '{}' is already registered. Overriding existing registration.",
                        id_str);
        }

        auto scene_info = std::make_unique<game_scene_info>();
        scene_info->scene_id = id_str;
        scene_info->state = game_scene_lifetime::unloaded;
        scene_info->scene_state = nullptr;
        scene_info->scene = scene;
        scene_info->engine = m_engine;

        m_scenes[id_str] = std::move(scene_info);

        log_info("Registered scene '{}'", id_str);
    }

    void game_scenes::unregister_scene(std::string_view scene_id) {
        auto id_str = std::string{scene_id};

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
        if (it->second->state != game_scene_lifetime::unloaded) {
            cleanup_scene_resources(it->second.get());
        }

        m_scenes.erase(it);

        log_info("Unregistered scene '{}'", id_str);
    }

    void game_scenes::load_scene(std::string_view scene_id, void* scene_state) {
        auto id_str = std::string{scene_id};

        auto it = m_scenes.find(id_str);
        if (it == m_scenes.end()) {
            throw error_message("Scene {} is not registered", id_str);
        }

        auto& scene_info = it->second;

        if (scene_info->state != game_scene_lifetime::unloaded) {
            log_warning("Scene '{}' is already loaded (state: {})", id_str,
                        static_cast<int>(scene_info->state));
            return;
        }

        log_info("Loading scene '{}'", id_str);

        scene_info->state = game_scene_lifetime::loading;
        scene_info->scene_state = scene_state;

        // Initialize scene systems
        scene_info->entities = std::make_unique<game_entities>();
        // The engine accessor returns a pointer; ensure it's valid and pass a reference
        game_renderer* renderer = m_engine->get_renderer();
        ensure(renderer != nullptr, "Renderer pointer is null when creating scene resources");
        scene_info->resources = std::make_unique<game_resources>(*renderer);

        // Create default camera and viewport
        create_default_camera_viewport(scene_info.get());

        safe_invoke(scene_info->scene.on_load, scene_info.get());

        // Scene is now loaded but not active yet
        scene_info->state = game_scene_lifetime::paused;
        log_info("Scene '{}' loaded successfully", id_str);
    }

    void game_scenes::unload_scene(std::string_view scene_id) {
        auto id_str = std::string{scene_id};

        auto it = m_scenes.find(id_str);
        if (it == m_scenes.end()) {
            log_warning("Attempted to unload non-existent scene '{}'", id_str);
            return;
        }

        auto& scene_info = it->second;

        if (scene_info->state == game_scene_lifetime::unloaded) {
            log_warning("Scene '{}' is already unloaded", id_str);
            return;
        }

        // If this is the active scene, deactivate it first
        if (m_active_scene_id == id_str) {
            deactivate_current_scene();
        }

        log_info("Unloading scene '{}'", id_str);
        scene_info->state = game_scene_lifetime::unloading;

        safe_invoke(scene_info->scene.on_unload, scene_info.get());

        cleanup_scene_resources(scene_info.get());
        scene_info->state = game_scene_lifetime::unloaded;

        log_info("Scene '{}' unloaded successfully", id_str);
    }

    void game_scenes::activate_scene(std::string_view scene_id) {
        std::string id_str{scene_id};

        auto it = m_scenes.find(id_str);
        if (it == m_scenes.end()) {
            throw error_message("Scene '{}' is not registered", id_str);
        }

        auto& scene_info = it->second;

        if (scene_info->state == game_scene_lifetime::unloaded) {
            throw error_message("Scene '{}' must be loaded before activation", id_str);
        }

        if (scene_info->state == game_scene_lifetime::active) {
            log_warning("Scene '{}' is already active", id_str);
            return;
        }

        // Deactivate current scene if one is active
        if (has_active_scene()) {
            deactivate_current_scene();
        }

        log_info("Activating scene '{}'", id_str);

        safe_invoke(scene_info->scene.on_activate, scene_info.get());

        scene_info->state = game_scene_lifetime::active;
        m_active_scene_id = id_str;

        // Update renderer to use scene's camera and viewport
        update_renderer_for_active_scene();

        log_info("Scene '{}' activated successfully", id_str);
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
        if (scene_info->state != game_scene_lifetime::active) {
            log_error("Scene '{}' is not in active state (current: {}), cannot deactivate",
                      m_active_scene_id, static_cast<int>(scene_info->state));
            return;
        }

        log_info("Deactivating scene '{}'", m_active_scene_id);

        safe_invoke(scene_info->scene.on_deactivate, scene_info.get());

        scene_info->state = game_scene_lifetime::paused;
        m_active_scene_id.clear();

        log_info("Scene deactivated successfully");
    }

    void game_scenes::switch_to_scene(std::string_view scene_id, void* scene_state) {
        load_scene(scene_id, scene_state);
        activate_scene(scene_id);
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

        auto name_str = std::string{name};
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

    void game_scenes::add_camera(std::string_view name, const glm::vec2& position,
                                 const float zoom) {
        auto* active_scene = get_active_scene();
        if (!active_scene) {
            throw std::runtime_error("No active scene available for camera management");
        }

        std::string name_str{name};

        if (active_scene->cameras.find(name_str) != active_scene->cameras.end()) {
            log_warning("Camera '{}' already exists in active scene. Overriding existing camera.",
                        name_str);
        }

        active_scene->cameras[name_str] = std::make_unique<game_camera>(name, position, zoom);
        log_info("Added camera '{}' to active scene '{}'", name_str, active_scene->scene_id);
    }

    void game_scenes::remove_camera(std::string_view name) {
        auto* active_scene = get_active_scene();
        if (!active_scene) {
            throw std::runtime_error("No active scene available for camera management");
        }

        std::string name_str{name};

        // Prevent removal of default camera
        if (name_str == game_scene_info::default_camera_name) {
            throw std::runtime_error("Cannot remove default '" + name_str + "' camera from scene");
        }

        auto it = active_scene->cameras.find(name_str);
        if (it == active_scene->cameras.end()) {
            log_warning("Attempted to remove non-existent camera '{}' from active scene", name_str);
            return;
        }

        active_scene->cameras.erase(it);
        log_info("Removed camera '{}' from active scene '{}'", name_str, active_scene->scene_id);
    }

    bool game_scenes::has_camera(std::string_view name) const {
        const auto* active_scene = get_active_scene();
        if (!active_scene) {
            return false;
        }

        return active_scene->cameras.find(std::string{name}) != active_scene->cameras.end();
    }

    void game_scenes::add_viewport(std::string_view name, const glm::vec2& position,
                                   const glm::vec2& size) {
        auto* active_scene = get_active_scene();
        if (!active_scene) {
            throw std::runtime_error("No active scene available for viewport management");
        }

        std::string name_str{name};

        if (active_scene->viewports.find(name_str) != active_scene->viewports.end()) {
            log_warning(
                "Viewport '{}' already exists in active scene. Overriding existing viewport.",
                name_str);
        }

        auto viewport = std::make_unique<game_viewport>(size);
        viewport->set_normalized_position(position);
        active_scene->viewports[name_str] = std::move(viewport);

        log_info("Added viewport '{}' to active scene '{}'", name_str, active_scene->scene_id);
    }

    void game_scenes::remove_viewport(std::string_view name) {
        auto* active_scene = get_active_scene();
        if (!active_scene) {
            throw std::runtime_error("No active scene available for viewport management");
        }

        std::string name_str{name};

        // Prevent removal of default viewport
        if (name_str == game_scene_info::default_viewport_name) {
            throw std::runtime_error("Cannot remove default '" + name_str +
                                     "' viewport from scene");
        }

        auto it = active_scene->viewports.find(name_str);
        if (it == active_scene->viewports.end()) {
            log_warning("Attempted to remove non-existent viewport '{}' from active scene",
                        name_str);
            return;
        }

        active_scene->viewports.erase(it);
        log_info("Removed viewport '{}' from active scene '{}'", name_str, active_scene->scene_id);
    }

    bool game_scenes::has_viewport(std::string_view name) const {
        const auto* active_scene = get_active_scene();
        if (!active_scene) {
            return false;
        }

        return active_scene->viewports.find(std::string{name}) != active_scene->viewports.end();
    }

    void game_scenes::for_each_scene(void (*callback)(const std::string&,
                                                      const game_scene_info&)) const {
        if (!callback) {
            log_error("Null callback provided to for_each_scene");
            return;
        }

        for (const auto& [scene_id, scene_info] : m_scenes) {
            callback(scene_id, *scene_info);
        }
    }

    void game_scenes::on_tick(const float tick_interval) {
        auto* active_scene = get_active_scene();
        if (!active_scene) {
            return;
        }

        safe_invoke(active_scene->scene.on_tick, active_scene, tick_interval);
    }

    void game_scenes::on_frame(const float frame_interval) {
        auto* active_scene = get_active_scene();
        if (!active_scene) {
            return;
        }

        safe_invoke(active_scene->scene.on_frame, active_scene, frame_interval);
    }

    void game_scenes::on_draw(const float fraction_to_next_tick) {
        auto* active_scene = get_active_scene();
        if (!active_scene) {
            return;
        }

        safe_invoke(active_scene->scene.on_draw, active_scene, fraction_to_next_tick);
    }

    void game_scenes::on_input() {
        auto* active_scene = get_active_scene();
        if (!active_scene) {
            return;
        }

        safe_invoke(active_scene->scene.on_input, active_scene);
    }

    void game_scenes::create_default_camera_viewport(game_scene_info* scene_info) {
        // Create default main camera with default position (0,0) and zoom (1.0)
        std::string main_camera{game_scene_info::default_camera_name};
        scene_info->cameras[main_camera] = std::make_unique<game_camera>(
            game_scene_info::default_camera_name, glm::vec2{0.0f, 0.0f}, 1.0f);

        // Create default main viewport
        std::string main_viewport{game_scene_info::default_viewport_name};
        scene_info->viewports[main_viewport] = std::make_unique<game_viewport>(glm::vec2{1.f, 1.f});
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

    void game_scenes::update_renderer_for_active_scene() {
        auto* active_scene = get_active_scene();
        if (!active_scene) {
            return;
        }

        game_renderer* renderer = m_engine->get_renderer();
        if (!renderer) {
            log_error("Cannot update renderer for active scene: renderer is null");
            return;
        }

        // Set the scene's default camera as active on the renderer
        auto camera_it = active_scene->cameras.find(game_scene_info::default_camera_name.data());
        if (camera_it != active_scene->cameras.end()) {
            renderer->set_camera(camera_it->second.get());
        }

        // Set the scene's default viewport as active on the renderer
        auto viewport_it =
            active_scene->viewports.find(game_scene_info::default_viewport_name.data());
        if (viewport_it != active_scene->viewports.end()) {
            renderer->set_viewport(viewport_it->second.get());
        }
    }

    void game_scenes::reset_renderer_to_global() {
        game_renderer* renderer = m_engine->get_renderer();
        if (!renderer) {
            log_error("Cannot reset renderer to global: renderer is null");
            return;
        }

        renderer->set_camera(nullptr);
        renderer->set_viewport(nullptr);
    }
}  // namespace engine
