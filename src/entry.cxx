#include <stdexcept>
#include <SDL3/SDL_main.h>
#include "engine/engine.hxx"

struct game_state {
    glm::vec2 player_position;
};

void game_create(engine::game_engine* engine) {
    game_state* state = engine->get_game_instance<game_state>();
    state->player_position = {100, 100};
}

void game_destroy(engine::game_engine* engine) {
    // Cleanup code here
}

void game_process_events(engine::game_engine* engine, SDL_Event* event) {
    game_state* state = engine->get_game_instance<game_state>();
    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.key == SDLK_W) {
            state->player_position.y -= 5;
        }
        if (event->key.key == SDLK_A) {
            state->player_position.x -= 5;
        }
        if (event->key.key == SDLK_S) {
            state->player_position.y += 5;
        }
        if (event->key.key == SDLK_D) {
            state->player_position.x += 5;
        }
    }
}

void game_render(engine::game_engine* engine) {
    engine::renderer& renderer = engine->get_renderer();
    game_state* state = engine->get_game_instance<game_state>();

    renderer.draw_rect({state->player_position.x, state->player_position.y}, {10, 10},
                       {255, 0, 0, 255});
}

int main(int argc, char* argv[]) {
    try {
        engine::game_callbacks callbacks;
        callbacks.create = game_create;
        callbacks.destroy = game_destroy;
        callbacks.process_events = game_process_events;
        callbacks.render = game_render;

        game_state state;

        engine::game_engine game_engine("My Epic Game", {800, 600}, &state, callbacks);
        game_engine.run();
    } catch (const std::exception& e) {
        SDL_Log("Error: %s", e.what());
        return 1;
    }

    return 0;
}