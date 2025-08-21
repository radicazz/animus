#include <stdexcept>
#include <SDL3/SDL_main.h>
#include "engine/engine.hxx"

struct game_state {
    glm::vec2 player_position;
    float player_speed;

    SDL_Texture* player_texture;
};

void game_create(engine::game_engine* engine) {
    game_state* state = engine->get_state<game_state>();
    state->player_position = {100, 100};
    state->player_speed = 200.f;

    engine::resource_manager& resource_manager = engine->get_resource_manager();
    state->player_texture = resource_manager.load_texture("assets/player/player_default.png");
}

void game_destroy(engine::game_engine* engine) {
    game_state* state = engine->get_state<game_state>();

    // Unnesecary because resource_manager will unload all textures and fonts when destroyed.
    engine::resource_manager& resource_manager = engine->get_resource_manager();
    resource_manager.unload_texture("assets/player/player_default.png");
}

void game_update(engine::game_engine* engine, float delta_time) {
    game_state* state = engine->get_state<game_state>();
    engine::input_manager& input_manager = engine->get_input_manager();

    glm::vec2 movement = input_manager.get_movement_input();
    state->player_position += movement * state->player_speed * delta_time;
}

void game_render(engine::game_engine* engine) {
    engine::renderer& renderer = engine->get_renderer();
    game_state* state = engine->get_state<game_state>();

    if (state->player_texture) {
        SDL_FRect dest = {state->player_position.x, state->player_position.y, 64, 64};
        SDL_RenderTexture(renderer.get_sdl_renderer(), state->player_texture, nullptr, &dest);
    }
}

int main(int argc, char* argv[]) {
    try {
        engine::game_callbacks callbacks;
        callbacks.create = game_create;
        callbacks.destroy = game_destroy;
        callbacks.update = game_update;
        callbacks.render = game_render;

        game_state state = {};
        engine::game_details details = {.title = "My Epic Game", .size = {800, 600}};

        engine::game_engine game_engine(details, &state, callbacks);
        game_engine.run();
    } catch (const std::exception& e) {
        SDL_Log("Error: %s", e.what());
        return 1;
    }

    return 0;
}