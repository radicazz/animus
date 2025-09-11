#pragma once

#include "../engine/engine.hxx"

struct demo_game_state {
    entt::entity player;
    entt::entity player_label;

    entt::entity asteroid;

    entt::entity camera_mode_text;

    bool is_free_camera;
    float free_camera_speed;
};

void game_on_create(engine::game_engine* engine);
void game_on_tick(engine::game_engine* engine, float tick_interval);
void game_on_frame(engine::game_engine* engine, float frame_interval);
void game_on_draw(engine::game_engine* engine, float progress_to_next_tick);
