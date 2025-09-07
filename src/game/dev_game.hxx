#pragma once

#include "../engine/engine.hxx"

// Just a simple example of a game's state and structure showing off engine features during
// development of the project.
struct dev_game_state {
    entt::entity player_entity{entt::null};
    entt::entity asteroid_entity{entt::null};

    glm::vec2 debug_text_position;
    std::unique_ptr<engine::render_text_static> debug_text;
    std::unique_ptr<engine::render_text_dynamic> player_label_text;

    // Camera control variables (game-specific behavior, not bounds)
    float camera_follow_speed;

    // Free camera mode toggle
    bool is_camera_free_mode;
    float camera_move_speed;
};

void game_create(engine::game_engine* engine);
void game_destroy(engine::game_engine* engine);
void game_fixed_update(engine::game_engine* engine, float fixed_delta_time);
void game_update(engine::game_engine* engine, float delta_time);
void game_render(engine::game_engine* engine);
