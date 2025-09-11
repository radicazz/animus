#include "game.hxx"

void game_on_create(engine::game_engine* engine) {
    auto& state = engine->get_state<demo_game_state>();
    engine::game_resources& resources = engine->get_resources();
    engine::game_entities& entities = engine->get_entities();

    engine::game_sprite::uptr player_sprite =
        resources.sprite_create("assets/sprites/player/default.png");
    state.player = entities.create_sprite_interpolated({200, 200}, std::move(player_sprite));

    state.player_label = resources.text_create_dynamic("assets/fonts/Segoe UI.ttf", 64.0f);
    state.player_label->set_text("player");
    state.player_label->set_origin_centered();
    state.player_label->set_scale(0.25f);

    engine::game_sprite::uptr asteroid_sprite =
        resources.sprite_create("assets/sprites/asteroids/ice_1.png", {64, 64});
    state.asteroid = entities.create_sprite_interpolated({300, 300}, std::move(asteroid_sprite));
    entities.set_velocity_angular(state.asteroid, 90.f);

    state.is_free_camera = false;
    state.free_camera_speed = 300.f;

    state.debug_text = resources.text_create_static("assets/fonts/Segoe UI.ttf", 18.0f);
    state.debug_text->set_text("Camera Mode: Follow");
    state.debug_text_position = {10, 10};
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
    if (input.is_key_pressed(engine::input_key::c) == true) {
        state.is_free_camera = !state.is_free_camera;
    }

    // Zoom in and out with O and P keys.
    if (input.is_key_held(engine::input_key::o) == true) {
        camera.zoom_by(1.0f - frame_interval);
    }

    if (input.is_key_held(engine::input_key::p) == true) {
        camera.zoom_by(1.0f + frame_interval);
    }

    // Access and modify the player's velocity based on WASD input.
    if (auto* velocity = entities.try_get<engine::component_velocity_linear>(state.player);
        velocity != nullptr) {
        constexpr float acceleration = 300.0f;

        glm::vec2 movement_input = {0.0f, 0.0f};

        if (input.is_key_held(engine::input_key::a) == true) {
            movement_input.x -= 1.0f;
        }

        if (input.is_key_held(engine::input_key::d) == true) {
            movement_input.x += 1.0f;
        }

        if (input.is_key_held(engine::input_key::w) == true) {
            movement_input.y -= 1.0f;
        }

        if (input.is_key_held(engine::input_key::s) == true) {
            movement_input.y += 1.0f;
        }

        velocity->value += movement_input * acceleration * frame_interval;
    }

    if (state.is_free_camera == false) {
        const glm::vec2 target_position =
            entities.get_interpolated_position(state.player, engine->get_fraction_to_next_tick());

        camera.follow_target(target_position);
    } else {
        // Free camera mode: Move camera with arrow keys.
        glm::vec2 camera_movement = {0.0f, 0.0f};

        if (input.is_key_held(engine::input_key::arrow_up) == true) {
            camera_movement.y -= 1.0f;
        }

        if (input.is_key_held(engine::input_key::arrow_down) == true) {
            camera_movement.y += 1.0f;
        }

        if (input.is_key_held(engine::input_key::arrow_left) == true) {
            camera_movement.x -= 1.0f;
        }

        if (input.is_key_held(engine::input_key::arrow_right) == true) {
            camera_movement.x += 1.0f;
        }

        if (camera_movement.x != 0.0f || camera_movement.y != 0.0f) {
            camera_movement *= state.free_camera_speed * frame_interval;
            camera.move_position(camera_movement);
        }
    }

    if (input.is_key_pressed(engine::input_key::mouse_left) == true) {
        // Convert mouse position to world space.
        const glm::vec2 mouse_click_position =
            viewport.screen_to_world(camera, input.get_mouse_screen_position());

        // Move the asteroid to where we clicked.
        entities.set_position(state.asteroid, mouse_click_position);
    }

    // Update the debug text to show the current camera mode.
    state.debug_text->set_text("Camera Mode: {}", state.is_free_camera ? "Free" : "Follow");
    state.debug_text->set_origin_centered();
}

void game_on_draw(engine::game_engine* engine, float fraction_to_next_tick) {
    auto& state = engine->get_state<demo_game_state>();
    engine::game_renderer& renderer = engine->get_renderer();
    engine::game_entities& entities = engine->get_entities();

    // Draw all renderable ECS entities.
    entities.system_renderer_update(renderer, fraction_to_next_tick);

    const glm::vec2 player_position =
        entities.get_interpolated_position(state.player, fraction_to_next_tick);
    renderer.text_draw_world(state.player_label, player_position + glm::vec2{0.f, 30.f});

    // Render our overlay text at the top-middle of the screen.
    const glm::vec2 screen_size = renderer.get_output_size();
    renderer.text_draw_screen(state.debug_text, {screen_size.x * 0.5f, 10});
}
