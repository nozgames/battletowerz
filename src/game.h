//
//  Breakout - Copyright(c) 2025 NoZ Games, LLC
//

#pragma once

constexpr int SCENE_ALLOCATOR_SIZE = 32 * noz::MB;
constexpr int MAX_ENTITIES = 512;

constexpr float TILE_SIZE = 1.5f;
constexpr int WORLD_TILE_WIDTH = 20;
constexpr float WORLD_WIDTH = (f32)WORLD_TILE_WIDTH * TILE_SIZE;
constexpr float WORLD_BOTTOM = 0.0f;
constexpr float WORLD_LEFT = -(f32)WORLD_WIDTH * 0.5f;
constexpr float WORLD_RIGHT = (f32)WORLD_WIDTH * 0.5f;

constexpr float VIEW_LEFT = WORLD_LEFT - 1.0f;
constexpr float VIEW_RIGHT = WORLD_RIGHT + 1.0f;

constexpr int UI_REF_WIDTH = 1920;
constexpr int UI_REF_HEIGHT = 1080;

// @arrow
constexpr float ARCHER_SPEED = 15.0f;
constexpr float ARCHER_ACCEL = 400.0f;
constexpr float ARCHER_FRICTION = 200.0f;
constexpr float ARCHER_JUMP_VELOCITY = 20.0f;

// @bow
constexpr float BOW_OFFSET_X = 0.75f;
constexpr float BOW_OFFSET_Y = 0.5f;
constexpr float BOW_CHARGE_SOUND = 0.1f;
constexpr float BOW_CHARGE_SOUND_VOLUME = 0.25f;
constexpr float BOW_CHARGE_SOUND_PITCH = 1.0f;
constexpr float BOW_CHARGE_TIME = 0.6f;
constexpr float BOW_CHARGE_RATE = 1.0f / BOW_CHARGE_TIME;
constexpr float BOW_DECHARGE_RATE = 200.0f;
constexpr float BOW_CHARGE_SCALE = 0.5f;
constexpr float BOW_FIRE_SOUND_VOLUME = 0.5f;
constexpr float BOW_COOLDOWN = 0.25f;

// @arrow
constexpr float ARROW_SPEED = 10.0f;
constexpr float ARROW_SPEED_CHARGED = 50.0f;
constexpr float ARROW_IMPACT_SOUND_VOLUME = 0.5f;

constexpr int WORLD_MAX_TILES = 1024;

constexpr float MAX_FALL_SPEED = 100.0f;

constexpr float GRAVITY = -50.0f;


constexpr EventId EVENT_GAME_OVER = 1;
constexpr EventId EVENT_VICTORY = 2;

enum GameState
{
    GAME_STATE_LOADING,
    GAME_STATE_MAIN_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_COUNT
};

enum EntityType
{
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_COUNT
};

struct Entity;

struct EntityVtable
{
    void (*update)(Entity* entity);
    void (*render)(Entity* entity, const Mat3& transform);
};

struct Entity
{
    EntityType type;
    EntityVtable vtable;
    Vec2 position;
    Vec2 scale;
    float depth;
    float rotation;
    Animator animator;
};

union FatEntity
{
    Entity entity;
};

struct Game
{
    Allocator* scene_allocator;

    GameState state;
    Material* material;
    InputSet* input;
    InputSet* input_ui;
    Camera* camera;

    Material* background_material;
    Mesh* background_mesh;

    Mesh* ring_mesh_1_00;
    Mesh* ring_mesh_0_75;
    Mesh* ring_mesh_0_50;
    Mesh* ring_mesh_0_25;

    PoolAllocator* entity_allocator;

    bool quit;
};

extern Game g_game;

#include "game_assets.h"

// @entity
extern Entity* CreateEntity(EntityType type, const EntityVtable& vtable, const Vec2& position = VEC2_ZERO, float rotation=0.0f, const Vec2& scale=VEC2_ONE);
extern void UpdateAnimator(Entity* entity);
