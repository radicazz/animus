#pragma once

#include "../engine/engine.hxx"

/**
 * @brief Development game state structure.
 *
 * This is a very simple game state representation used for development during the game. The
 * structure along with it's callbacks provide an example for how this engine might be used.
 */
struct dev_game_state {
    glm::vec2 player_position;
    float player_speed;
};

void game_create(engine::game_engine* engine);
void game_destroy(engine::game_engine* engine);
void game_update(engine::game_engine* engine, float delta_time);
void game_render(engine::game_engine* engine);
