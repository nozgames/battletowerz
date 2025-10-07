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

enum Team {
    TEAM_RED,
    TEAM_BLUE,
    TEAM_COUNT
};

enum EntityType
{
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_ARCHER,
    ENTITY_TYPE_COUNT
};

struct Entity;

struct EntityVtable
{
    void (*update)(Entity* entity);
    void (*render)(Entity* entity, const Mat3& transform);
};

struct Entity {
    EntityType type;
    EntityVtable vtable;
    Vec2 position;
    Vec2 scale;
    float depth;
    float rotation;
    Animator animator;
};

struct ArcherEntity : Entity {
    Team team;
};

union FatEntity
{
    Entity entity;
    ArcherEntity archer;
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
    Mesh* quad_mesh;

    PoolAllocator* entity_allocator;

    bool quit;
};

extern Game g_game;

#include "game_assets.h"

// @entity
extern Entity* CreateEntity(EntityType type, const EntityVtable& vtable, const Vec2& position = VEC2_ZERO, float rotation=0.0f, const Vec2& scale=VEC2_ONE);
extern void UpdateAnimator(Entity* entity);

// @team
inline Vec2 GetTeamDirection(Team team) {
    static Vec2 directions[TEAM_COUNT] = {
        Vec2{-1,0},
        Vec2{1,0}
    };
    return directions[team];
}

inline Color GetTeamColor(Team team) {
    static Color colors[TEAM_COUNT] = {
        Color32ToColor(228, 92, 95, 255),
        Color32ToColor(85, 177, 241, 255)
    };
    return colors[team];
}

extern ArcherEntity* CreateArcher(Team team, const Vec2& position);