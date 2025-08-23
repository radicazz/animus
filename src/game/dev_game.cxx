#include "dev_game.hxx"

void game_create(engine::game_engine* engine) {
    auto& state = engine->get_state<dev_game_state>();
    state.player_position = {100, 100};
    state.player_speed = 200.f;

    engine::resource_manager& resource_manager = engine->get_resource_manager();
    state.sprite_player = resource_manager.sprite_load("assets/sprites/player/default.png");
}

void game_destroy(engine::game_engine*) {
}

void game_update(engine::game_engine* engine, float delta_time) {
    auto& state = engine->get_state<dev_game_state>();
    engine::input_system& input = engine->get_input_system();

    glm::vec2 movement = input.get_movement();
    state.player_position += movement * state.player_speed * delta_time;

    if (input.is_key_pressed(engine::input_key::mouse_left) == true) {
        glm::vec2 mouse_pos = input.get_mouse_pos();
        SDL_Log("Mouse Left Click at: (%f, %f)", mouse_pos.x, mouse_pos.y);
    }
}

void game_render(engine::game_engine* engine) {
    auto& state = engine->get_state<dev_game_state>();
    engine::renderer& renderer = engine->get_renderer();

    renderer.sprite_texture_draw(state.sprite_player, state.player_position);
}
