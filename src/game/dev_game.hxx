#pragma once

#include "../engine/engine.hxx"

// Just a simple example of a game's state and structure showing off engine features during
// development of the project.
struct dev_game_state {
    glm::vec2 player_position;
    float player_speed;
    std::unique_ptr<engine::render_sprite> player_sprite;

    glm::vec2 asteroid_position;
    std::unique_ptr<engine::render_sprite> asteroid_sprite;

    glm::vec2 debug_text_position;
    std::unique_ptr<engine::render_text_static> debug_text;

    std::unique_ptr<engine::render_text_dynamic> player_label_text;

    // Camera control variables (game-specific behavior, not bounds)
    float camera_follow_speed;

    // Free camera mode toggle
    bool is_camera_free_mode;
    float camera_move_speed;

    // Physics/fixed update variables
    glm::vec2 player_velocity{0.0f, 0.0f};
    float physics_gravity{-200.0f};  // Pixels per second squared
    float physics_friction{0.95f};   // Friction coefficient
    bool is_grounded{false};
};

void game_create(engine::game_engine* engine);
void game_destroy(engine::game_engine* engine);
void game_fixed_update(engine::game_engine* engine, float fixed_delta_time);
void game_update(engine::game_engine* engine, float delta_time);
void game_render(engine::game_engine* engine);
