#include <stdexcept>
#include <SDL3/SDL_main.h>
#include "engine/engine.hxx"

struct game_state {
    glm::vec2 player_position;
    SDL_Texture* player_texture;
};

void game_create(engine::game_engine* engine) {
    game_state* state = engine->get_state<game_state>();

    state->player_position = {100, 100};

    state->player_texture = IMG_LoadTexture(engine->get_renderer().get_sdl_renderer(),
                                            "assets/player/player_default.png");
}

void game_destroy(engine::game_engine* engine) {
    game_state* state = engine->get_state<game_state>();

    if (state->player_texture) {
        SDL_DestroyTexture(state->player_texture);
    }
}

void game_process_events(engine::game_engine* engine, SDL_Event* event) {
    game_state* state = engine->get_state<game_state>();

    // Track key states for smooth diagonal movement
    static bool keys[4] = {false};  // W, A, S, D

    if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
        bool is_down = (event->type == SDL_EVENT_KEY_DOWN);

        switch (event->key.key) {
            case SDLK_W:
                keys[0] = is_down;
                break;
            case SDLK_A:
                keys[1] = is_down;
                break;
            case SDLK_S:
                keys[2] = is_down;
                break;
            case SDLK_D:
                keys[3] = is_down;
                break;
        }

        // Calculate movement vector
        glm::vec2 movement(0.0f);

        if (keys[0])
            movement.y -= 1.0f;  // W
        if (keys[1])
            movement.x -= 1.0f;  // A
        if (keys[2])
            movement.y += 1.0f;  // S
        if (keys[3])
            movement.x += 1.0f;  // D

        // Normalize diagonal movement to prevent faster diagonal speed
        if (movement.x != 0.0f && movement.y != 0.0f) {
            movement = glm::normalize(movement);
        }

        // Apply movement with consistent speed
        const float speed = 5.0f;
        state->player_position += movement * speed;
    }
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
        callbacks.process_events = game_process_events;
        callbacks.render = game_render;

        game_state state;
        engine::game_details details = {.title = "My Epic Game", .size = {800, 600}};

        engine::game_engine game_engine(details, &state, callbacks);
        game_engine.run();
    } catch (const std::exception& e) {
        SDL_Log("Error: %s", e.what());
        return 1;
    }

    return 0;
}