#include <stdexcept>
#include <SDL3/SDL_main.h>

#include "game/dev_game.hxx"

int main(int argc, char* argv[]) {
    try {
        // Game state object.
        dev_game_state state = {};

        // Callbacks that the engine exposes to the game developer.
        engine::game_callbacks callbacks = {.on_create = game_create,
                                            .on_destroy = game_destroy,
                                            .on_update = game_update,
                                            .on_fixed_update = game_fixed_update,
                                            .on_render = game_render};

        // Initial game engine window parameters such as title and size.
        engine::game_details details = {.window_title = "Incarnate Window",
                                        .window_size = glm::vec2{800, 600}};

        // Create the game and its resources.
        engine::game_engine game_engine(details, &state, callbacks);

        // Run the application loop.
        game_engine.run();
    } catch (const std::exception& e) {
        SDL_Log("Error: %s", e.what());
        return 1;
    }

    return 0;
}