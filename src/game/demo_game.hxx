#pragma once

#include "../engine/engine.hxx"

struct demo_game_state {
    entt::entity player;
    engine::game_text_dynamic::uptr player_label;

    entt::entity asteroid;

    glm::vec2 debug_text_position;
    engine::game_text_static::uptr debug_text;

    bool is_free_camera;
    float free_camera_speed;
};

void game_create(engine::game_engine* engine);
void game_fixed_update(engine::game_engine* engine, float fixed_delta_time);
void game_update(engine::game_engine* engine, float delta_time);
void game_render(engine::game_engine* engine, float interpolation_alpha);
