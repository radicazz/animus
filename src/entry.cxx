#include <stdexcept>
#include <SDL3/SDL_main.h>

#include "game/dev_game.hxx"

int main(int argc, char* argv[]) {
    try {
        // Callbacks that the engine exposes to the game developer.
        engine::game_callbacks callbacks = {.create = game_create,
                                            .destroy = game_destroy,
                                            .update = game_update,
                                            .render = game_render};

        // Initial game engine window parameters such as title and size.
        engine::game_details details = {.title = "My Epic Game", .size = glm::vec2{800, 600}};

        // Game state object.
        dev_game_state state = {};

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