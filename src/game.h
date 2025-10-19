//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
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

#include "entity.h"

enum GameState
{
    GAME_STATE_LOADING,
    GAME_STATE_MAIN_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_COUNT
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

    Vec2 mouse_position;
    Vec2 pan_position;
    Vec2 pan_position_camera;
    Vec2 mouse_world_position;
};

extern Game g_game;

#include "game_assets.h"

// @game
extern void SetGameState(GameState state);
extern bool IsGameState(GameState state);

// @entity
extern Entity* CreateEntity(EntityType type, const EntityVtable& vtable, const Vec2& position = VEC2_ZERO, float rotation=0.0f, const Vec2& scale=VEC2_ONE);
extern void UpdateAnimator(Entity* entity);

// @world
extern void InitWorld();
extern void DrawWorld(Camera* camera);

// @colors
constexpr Color BACKGROUND_COLOR = Color32ToColor(240,240,240,255);
constexpr Color VIGNETTE_COLOR = Color32ToColor(148,148,148,255);
constexpr Color FOREGROUND_COLOR = Color32ToColor(88,88,88,255);
constexpr Color HOVER_COLOR = Color32ToColor(85, 177, 241,255);
constexpr Color GRID_COLOR = {0.4f, 0.4f, 0.4f, 0.1f};

// @menu
extern void UpdateMenu();
extern void InitMenu();
extern void OpenMainMenu();
extern void OpenPauseMenu();

// @edit
extern void InitEditor();
extern void UpdateEditorUI();
extern void UpdateEditor();
