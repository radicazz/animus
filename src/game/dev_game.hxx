#pragma once

#include "../engine/engine.hxx"

// Just a simple example of a game's state and structure showing off engine features during
// development of the project.
struct dev_game_state {
    glm::vec2 player_position;
    float player_speed;

    std::unique_ptr<engine::render_sprite> player_sprite;
    std::unique_ptr<engine::render_sprite> asteroid_sprite;

    std::unique_ptr<engine::render_text> example_text;

    // Camera control variables (game-specific behavior, not bounds)
    float camera_follow_speed;

    // Free camera mode toggle
    bool is_camera_free_mode;
    float camera_move_speed;
};

void game_create(engine::game_engine* engine);
void game_destroy(engine::game_engine* engine);
void game_update(engine::game_engine* engine, float delta_time);
void game_render(engine::game_engine* engine);
