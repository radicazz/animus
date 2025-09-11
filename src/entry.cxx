#include <stdexcept>
#include <SDL3/SDL_main.h>

#include "game/game.hxx"

static void game_run() {
    demo_game_state state = {};

    // Callbacks that the engine exposes to the game developer
    engine::game_info info = {.state = &state,
                              .on_create = game_on_create,
                              .on_destroy = nullptr,
                              .on_tick = game_on_tick,
                              .on_frame = game_on_frame,
                              .on_draw = game_on_draw};

    // Create the game and its resources
    engine::game_engine game(info, "Incarnate Demo", {1000, 700});
    game.run();
}

int main(int argc, char* argv[]) {
    try {
        game_run();
    } catch (const std::exception& e) {
        SDL_Log("Error: %s", e.what());
        return 1;
    }

    return 0;
}