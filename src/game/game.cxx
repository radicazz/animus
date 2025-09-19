#include "game.hxx"

void game_on_create(engine::game_engine* engine) {
    auto& state = engine->get_state<demo_game_state>();
    engine::game_resources& resources = engine->get_resources();
    engine::game_entities& entities = engine->get_entities();

    // Create player sprite as resource-managed sprite
    auto* player_sprite =
        resources.sprite_get_or_create("player_sprite", "assets/sprites/player/default.png");
    player_sprite->set_origin({16, 24});
    state.player = entities.sprite_create_interpolated("player_sprite");
    entities.set_transform_position(state.player, {200, 200});
    entities.set_velocity_linear_drag(state.player, 0.3f);
    entities.set_velocity_linear_max(state.player, 500.f);
    entities.set_velocity_angular_drag(state.player, 0.3f);
    entities.set_velocity_angular_max(state.player, 360.f);

    // Create player label as resource-managed text
    auto* player_label = resources.text_dynamic_get_or_create(
        "player_label", "player", "assets/fonts/roboto_regular.ttf", 64.0f);
    player_label->set_origin_centered();
    state.player_label = entities.create_text_dynamic("player_label");
    entities.set_transform_position(state.player_label, {200, 230});
    entities.set_transform_scale(state.player_label, {0.25f, 0.25f});

    // Create asteroid sprite as resource-managed sprite
    auto* asteroid_sprite =
        resources.sprite_get_or_create("asteroid_sprite", "assets/sprites/asteroids/ice_1.png");
    asteroid_sprite->set_size({64, 64});
    asteroid_sprite->set_origin(asteroid_sprite->get_size() * 0.5f);
    state.asteroid = entities.sprite_create_interpolated("asteroid_sprite");
    entities.set_transform_position(state.asteroid, {400, 200});
    entities.set_velocity_angular(state.asteroid, 90.f);

    state.is_free_camera = false;
    state.free_camera_speed = 300.f;

    resources.text_static_get_or_create("camera_mode_text", "Camera Mode: Follow",
                                        "assets/fonts/roboto_regular.ttf", 18.0f);
}

void game_on_tick(engine::game_engine* engine, const float tick_interval) {
    engine::game_entities& entities = engine->get_entities();
    entities.system_lifetime_update(tick_interval);
    entities.system_physics_update(tick_interval);
}

void game_on_frame(engine::game_engine* engine, const float frame_interval) {
    auto& state = engine->get_state<demo_game_state>();
    engine::game_input& input = engine->get_input();
    engine::game_camera& camera = engine->get_camera();
    engine::game_viewport& viewport = engine->get_viewport();
    engine::game_entities& entities = engine->get_entities();

    // Toggle camera mode with the 'C' key.
    if (input.is_key_pressed(engine::game_input_key::c) == true) {
        state.is_free_camera = !state.is_free_camera;
    }

    // Zoom in and out with O and P keys.
    if (input.is_key_held(engine::game_input_key::o) == true) {
        camera.zoom_by(1.0f - frame_interval);
    }

    if (input.is_key_held(engine::game_input_key::p) == true) {
        camera.zoom_by(1.0f + frame_interval);
    }

    constexpr float player_acceleration = 250.f;

    const glm::vec2 movement_input = input.get_movement_wasd();

    if (movement_input.x != 0.0f) {
        entities.add_impulse_right(state.player,
                                   movement_input.x * player_acceleration * frame_interval);
    }

    if (movement_input.y != 0.0f) {
        entities.add_impulse_forward(state.player,
                                     movement_input.y * player_acceleration * frame_interval);
    }

    if (state.is_free_camera == false) {
        const glm::vec2 target_position =
            entities.get_interpolated_position(state.player, engine->get_fraction_to_next_tick());

        camera.follow_target(target_position);
    } else {
        camera.move_position(input.get_movement_arrows() * state.free_camera_speed *
                             frame_interval);
    }

    if (input.is_key_pressed(engine::game_input_key::mouse_left) == true) {
        // Convert mouse position to world space.
        const glm::vec2 mouse_click_position =
            viewport.screen_to_world(camera, input.get_mouse_position());

        // Move the asteroid to where we clicked.
        entities.set_transform_position(state.asteroid, mouse_click_position);
    }

    // Update player label to follow player
    const glm::vec2 player_position =
        entities.get_interpolated_position(state.player, engine->get_fraction_to_next_tick());
    entities.set_transform_position(state.player_label, player_position + glm::vec2{0.f, 30.f});
}

void game_on_draw(engine::game_engine* engine, float fraction_to_next_tick) {
    auto& state = engine->get_state<demo_game_state>();
    engine::game_renderer& renderer = engine->get_renderer();
    engine::game_resources& resources = engine->get_resources();
    engine::game_entities& entities = engine->get_entities();

    // Update and render the camera mode text indicator to the screen (UI overlay).
    if (auto* camera_text = resources.text_static_get("camera_mode_text")) {
        camera_text->set_text("Camera Mode: {}", state.is_free_camera ? "Free" : "Follow");
        camera_text->set_origin_centered();
        const glm::vec2 output_size = renderer.get_output_size();
        renderer.text_draw_screen(camera_text, {output_size.x * 0.5f, 20.f});
    }

    // Draw all renderable ECS entities (sprites and world-space text).
    entities.system_renderer_update(renderer, resources, fraction_to_next_tick);
}
