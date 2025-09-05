#include "dev_game.hxx"

void game_create(engine::game_engine* engine) {
    auto& state = engine->get_state<dev_game_state>();
    state.player_position = {100, 100};
    state.player_speed = 200.f;

    // Camera behavior settings (not bounds - those are managed by engine)
    state.camera_follow_speed = 0.05f; // Smooth camera following
    
    // Free camera settings
    state.is_camera_free_mode = false;  // Start in follow mode
    state.camera_move_speed = 300.f; // Camera movement speed in free mode

    engine::resource_manager& resource_manager = engine->get_resource_manager();
    state.player_sprite = resource_manager.sprite_load("assets/sprites/player/default.png");
    state.asteroid_sprite =
        resource_manager.sprite_load("assets/sprites/asteroids/ice_1.png", {64, 64});
}

void game_destroy(engine::game_engine*) {
    // RAII currently handles resource cleanup.
    // Nothing to do here.
}

void game_update(engine::game_engine* engine, float delta_time) {
    auto& state = engine->get_state<dev_game_state>();
    engine::input_system& input = engine->get_input_system();
    engine::camera& camera = engine->get_camera();

    // Toggle camera mode with 'C' key
    if (input.is_key_pressed(engine::input_key::c)) {
        state.is_camera_free_mode = !state.is_camera_free_mode;
    }

    // Player movement (always active regardless of camera mode)
    glm::vec2 movement(0.0f);
    if (input.is_key_held(engine::input_key::w)) {
        movement.y -= 1.0f;
    }
    if (input.is_key_held(engine::input_key::s)) {
        movement.y += 1.0f;
    }
    if (input.is_key_held(engine::input_key::a)) {
        movement.x -= 1.0f;
    }
    if (input.is_key_held(engine::input_key::d)) {
        movement.x += 1.0f;
    }
    state.player_position += movement * state.player_speed * delta_time;

    // Keep player within the same bounds as the camera uses
    // Query the engine-managed world bounds
    if (camera.has_bounds()) {
        glm::vec2 bounds_min = camera.get_bounds_min();
        glm::vec2 bounds_max = camera.get_bounds_max();
        state.player_position.x = glm::clamp(state.player_position.x, bounds_min.x, bounds_max.x);
        state.player_position.y = glm::clamp(state.player_position.y, bounds_min.y, bounds_max.y);
    }

    // Camera zoom controls - 'o' to zoom out, 'p' to zoom in
    if (input.is_key_held(engine::input_key::o)) {
        camera.zoom_by(1.0f - delta_time);
    }
    if (input.is_key_held(engine::input_key::p)) {
        camera.zoom_by(1.0f + delta_time);
    }

    // Camera control based on mode
    if (state.is_camera_free_mode) {
        // FREE CAMERA MODE: Use arrow keys to move camera independently
        glm::vec2 camera_movement(0.0f);
        if (input.is_key_held(engine::input_key::arrow_up)) {
            camera_movement.y -= 1.0f;
        }
        if (input.is_key_held(engine::input_key::arrow_down)) {
            camera_movement.y += 1.0f;
        }
        if (input.is_key_held(engine::input_key::arrow_left)) {
            camera_movement.x -= 1.0f;
        }
        if (input.is_key_held(engine::input_key::arrow_right)) {
            camera_movement.x += 1.0f;
        }
        
        // Move camera independently of player
        if (camera_movement.x != 0.0f || camera_movement.y != 0.0f) {
            camera_movement *= state.camera_move_speed * delta_time;
            camera.move_position(camera_movement);
        }
    } else {
        // FOLLOW MODE: Camera follows the player
        camera.follow_target(state.player_position, state.camera_follow_speed);
    }

    // Rotate asteroids
    state.asteroid_sprite->set_rotation(state.asteroid_sprite->get_rotation() + 36.f * delta_time);
}

void game_render(engine::game_engine* engine) {
    auto& state = engine->get_state<dev_game_state>();
    engine::renderer& renderer = engine->get_renderer();

    // Draw sprites in world coordinates - camera will handle the transformation
    renderer.sprite_draw_world(state.player_sprite, state.player_position);

    // Draw a grid of asteroids to see camera movement
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            glm::vec2 grid_position = {x * 150.0f, y * 150.0f};
            renderer.sprite_draw_world(state.asteroid_sprite, grid_position);
        }
    }
}
