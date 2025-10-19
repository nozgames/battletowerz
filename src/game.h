//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#pragma once

constexpr int SCENE_ALLOCATOR_SIZE = 32 * noz::MB;
constexpr int MAX_UNITS = 1024;
constexpr int MAX_PROJECTILES = 2048;
constexpr int MAX_ENTITIES = MAX_UNITS + MAX_PROJECTILES;

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

enum GameState {
    GAME_STATE_LOADING,
    GAME_STATE_MAIN_MENU,
    GAME_STATE_EDIT,
    GAME_STATE_BATTLE,
    GAME_STATE_GAME_OVER,
    GAME_STATE_PAUSE,
    GAME_STATE_COUNT
};

struct UnitSetup {
    const UnitInfo* unit_info;
    Vec2 position;
    Team team;
};

struct BattleSetup {
    UnitSetup units[MAX_UNITS];
    int unit_count;
};

struct Game {
    Allocator* scene_allocator;

    GameState state;
    Material* material;
    InputSet* input;
    InputSet* input_ui;
    Camera* camera;

    Material* background_material;
    Material* shadow_material;
    Mesh* background_mesh;
    Mesh* quad_mesh;
    Mesh* line_mesh;

    PoolAllocator* entity_allocator;

    bool quit;

    Vec2 mouse_position;
    Vec2 pan_position;
    Vec2 pan_position_camera;
    Vec2 mouse_world_position;

    BattleSetup battle_setup;
};

extern Game g_game;

#include "game_assets.h"

// @game
extern void SetGameState(GameState state);
extern bool IsGameState(GameState state);
extern bool IsPlaying();
extern float GetGameFrameTime();

// @entity
extern Entity* CreateEntity(EntityType type, const EntityVtable& vtable, const Vec2& position = VEC2_ZERO, float rotation=0.0f, const Vec2& scale=VEC2_ONE);
extern void UpdateAnimator(Entity* entity);
extern void DestroyAllEntities();

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
extern Color GetButtonBackgroundColor(ElementState state, float time, void* user_data);

// @edit
extern void InitEditor();
extern void DrawEditor();
extern void UpdateEditorUI();
extern void UpdateEditor();

// @battle
extern void StartBattle(const BattleSetup& setup);
extern void UpdateBattle();
extern void UpdateBattleUI();
extern void DrawBattle();
extern void HandleUnitDeath(UnitEntity* entity, DamageType damage_type);
