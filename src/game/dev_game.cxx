#include "dev_game.hxx"

void game_create(engine::game_engine* engine) {
    auto& state = engine->get_state<dev_game_state>();
    engine::game_resources& resource_manager = engine->get_resources();
    engine::ecs_manager& ecs = engine->get_ecs_manager();

    auto player_sprite = resource_manager.sprite_create("assets/sprites/player/default.png");
    state.player_entity = ecs.create_sprite_entity({300, 300}, std::move(player_sprite));
    ecs.add_component<engine::component_velocity>(state.player_entity) = {
        .linear = {0.0f, 0.0f}, .max_speed = 200.0f, .drag = 0.1f};

    // Asteroid
    auto asteroid_sprite =
        resource_manager.sprite_create("assets/sprites/asteroids/ice_1.png", {64, 64});
    state.asteroid_entity = ecs.create_sprite_entity({300, 300}, std::move(asteroid_sprite));

    // Camera
    state.camera_follow_speed = 0.05f;
    state.is_camera_free_mode = false;
    state.camera_move_speed = 300.f;

    // Static text
    state.debug_text = resource_manager.text_create_static("assets/fonts/Segoe UI.ttf", 18.0f);
    state.debug_text->set_text("Camera Mode: Follow");
    state.debug_text_position = {10, 10};

    // Dynamic text.
    state.player_label_text =
        resource_manager.text_create_dynamic("assets/fonts/Segoe UI.ttf", 64.0f);
    state.player_label_text->set_text("player");
    state.player_label_text->set_origin_centered();
    state.player_label_text->set_scale(0.25f);
}

void game_destroy(engine::game_engine*) {
    // RAII currently handles resource cleanup so nothing to really do here.
}

void game_fixed_update(engine::game_engine* engine, float fixed_delta_time) {
    auto& state = engine->get_state<dev_game_state>();
    engine::game_camera& camera = engine->get_camera();
    engine::ecs_manager& ecs = engine->get_ecs_manager();

    auto transform_view = ecs.get_entities_with<engine::component_transform_interpolated>();
    for (auto entity : transform_view) {
        auto& transform = transform_view.get<engine::component_transform_interpolated>(entity);
        // Store previous position and rotation before physics update for interpolation.
        transform.previous_position = transform.position;
        transform.previous_rotation_degrees = transform.rotation_degrees;
    }

    // Update various physics systems.
    ecs.update_physics(fixed_delta_time);
    ecs.update_lifetime(fixed_delta_time);
}

void game_update(engine::game_engine* engine, float delta_time) {
    auto& state = engine->get_state<dev_game_state>();
    engine::game_input& input = engine->get_input();
    engine::game_camera& camera = engine->get_camera();
    engine::ecs_manager& ecs = engine->get_ecs_manager();

    // Toggle camera mode with 'C' key.
    if (input.is_key_pressed(engine::input_key::c)) {
        state.is_camera_free_mode = !state.is_camera_free_mode;
    }

    if (ecs.is_valid(state.player_entity) == true) {
        auto* player_velocity =
            ecs.try_get_component<engine::component_velocity>(state.player_entity);
        if (player_velocity != nullptr) {
            constexpr float acceleration = 1000.0f;  // pixels per second squared

            glm::vec2 input_force{0.0f, 0.0f};
            if (input.is_key_held(engine::input_key::a)) {
                input_force.x -= acceleration;
            }
            if (input.is_key_held(engine::input_key::d)) {
                input_force.x += acceleration;
            }
            if (input.is_key_held(engine::input_key::w)) {
                input_force.y -= acceleration;
            }
            if (input.is_key_held(engine::input_key::s)) {
                input_force.y += acceleration;
            }

            player_velocity->linear += input_force * delta_time;
        }
    }

    // Camera zoom controls - 'o' to zoom out, 'p' to zoom in.
    if (input.is_key_held(engine::input_key::o)) {
        camera.zoom_by(1.0f - delta_time);
    }
    if (input.is_key_held(engine::input_key::p)) {
        camera.zoom_by(1.0f + delta_time);
    }

    if (state.is_camera_free_mode == true) {
        // FREE CAMERA MODE: Use arrow keys to move camera independently.
        glm::vec2 camera_movement = {0.0f, 0.0f};
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

        camera.follow_target(ecs.get_position(state.player_entity), state.camera_follow_speed);
    }

    auto* asteroid_transform =
        ecs.try_get_component<engine::component_transform_interpolated>(state.asteroid_entity);
    if (asteroid_transform != nullptr) {
        asteroid_transform->rotation_degrees += 45.f * delta_time;
        if (input.is_key_pressed(engine::input_key::mouse_left) == true) {
            asteroid_transform->position =
                camera.screen_to_world(input.get_mouse_screen_position());
        }
    }

    // Update the text.
    state.debug_text->set_text("Camera Mode: {}", state.is_camera_free_mode ? "Free" : "Follow");
    state.debug_text->set_origin_centered();
}

void game_render(engine::game_engine* engine, float interpolation_alpha) {
    auto& state = engine->get_state<dev_game_state>();
    engine::game_renderer& renderer = engine->get_renderer();
    engine::ecs_manager& ecs = engine->get_ecs_manager();

    ecs.render_sprites(engine, interpolation_alpha);

    if (ecs.is_valid(state.player_entity) == true) {
        glm::vec2 player_position = ecs.get_position(state.player_entity);
        renderer.text_draw_world(state.player_label_text.get(),
                                 player_position + glm::vec2{0.f, 30.f});
    }

    // Render our overlay text at the top-middle of the screen.
    const glm::vec2 screen_size = renderer.get_output_size();
    renderer.text_draw_screen(state.debug_text.get(), {screen_size.x * 0.5f, 10});
}
