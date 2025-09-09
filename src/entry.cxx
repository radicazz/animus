#include <stdexcept>
#include <SDL3/SDL_main.h>

#include "game/demo_game.hxx"

static void dev_game_run() {
    demo_game_state state = {};

    // Callbacks that the engine exposes to the game developer
    engine::game_info info = {.state = &state,
                              .on_create = game_create,
                              .on_destroy = nullptr,
                              .on_update = game_update,
                              .on_fixed_update = game_fixed_update,
                              .on_render = game_render};

    // Create the game and its resources
    engine::game_engine game(info, "Incarnate Demo", {1000, 700});

    // Run the application loop
    game.run();
}

int main(int argc, char* argv[]) {
    try {
        dev_game_run();
    } catch (const std::exception& e) {
        SDL_Log("Error: %s", e.what());
        return 1;
    }

    return 0;
}