#include "dev_game.hxx"

void game_create(engine::game_engine* engine) {
    auto& state = engine->get_state<dev_game_state>();
    state.player_position = {100, 100};
    state.player_speed = 200.f;

    engine::resource_manager& resource_manager = engine->get_resource_manager();

    state.player_sprite = resource_manager.sprite_load("assets/sprites/player/default.png");
    state.asteroid_sprite =
        resource_manager.sprite_load("assets/sprites/asteroids/ice_1.png", {64, 64});
}

void game_destroy(engine::game_engine*) {
}

void game_update(engine::game_engine* engine, float delta_time) {
    auto& state = engine->get_state<dev_game_state>();
    engine::input_system& input = engine->get_input_system();

    const glm::vec2 movement = input.get_movement();
    state.player_position += movement * state.player_speed * delta_time;

    state.asteroid_sprite->set_rotation(state.asteroid_sprite->get_rotation() + 36.f * delta_time);
}

void game_render(engine::game_engine* engine) {
    auto& state = engine->get_state<dev_game_state>();
    engine::renderer& renderer = engine->get_renderer();

    renderer.sprite_draw(state.player_sprite, state.player_position);
    renderer.sprite_draw(state.asteroid_sprite, {600, 400});
}
