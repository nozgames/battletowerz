//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#pragma once

constexpr int SCENE_ALLOCATOR_SIZE = 32 * noz::MB;
constexpr int MAX_UNITS = 1024;
constexpr int MAX_PROJECTILES = 2048;
constexpr int MAX_ENTITIES = MAX_UNITS + MAX_PROJECTILES;

constexpr float VIEW_HEIGHT = 20.0f;
constexpr float VIEW_MIN_HEIGHT = 5.0f;
constexpr float VIEW_MAX_HEIGHT = 100.0f;

constexpr float ZOOM_MIN = VIEW_HEIGHT / VIEW_MAX_HEIGHT;
constexpr float ZOOM_MAX = VIEW_HEIGHT / VIEW_MIN_HEIGHT;
constexpr float ZOOM_STEP = 0.1f;

constexpr int UI_REF_WIDTH = 1920;
constexpr int UI_REF_HEIGHT = 1080;

constexpr int WORLD_MAX_TILES = 1024;

constexpr float MAX_FALL_SPEED = 100.0f;

constexpr float GRAVITY = -5.0f;


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
    Vec3 position;
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
    float zoom;

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
    Vec3 mouse_world_position;

    BattleSetup battle_setup;

    float time_scale;
};

extern Game g_game;

#include "game_assets.h"

// @game
extern void SetGameState(GameState state);
extern bool IsGameState(GameState state);
extern bool IsPlaying();
extern float GetGameFrameTime();
extern void UpdateCameraZoom();
extern void UpdateCameraPan();
extern void ResetCamera();
extern void SetGameTimeScale(float time_scale);
inline float GetGameTimeScale() { return g_game.time_scale; }

// @world
extern void InitWorld();
extern void DrawWorld(Camera* camera);
extern void DrawGrid(Camera* camera);

// @colors
constexpr Color BACKGROUND_COLOR = Color32ToColor(220,220,220,255);
constexpr Color VIGNETTE_COLOR = Color32ToColor(210,210,210,255);
constexpr Color FOREGROUND_COLOR = Color32ToColor(88,88,88,255);
constexpr Color HOVER_COLOR = Color32ToColor(85, 177, 241,255);
constexpr Color GRID_COLOR = {0.4f, 0.4f, 0.4f, 0.1f};
constexpr Color UI_LETTERBOX_COLOR = {0.3f, 0.3f, 0.3f, 1.0f};
constexpr Color UI_LETTERBOX_BORDER_COLOR = {0.1f, 0.1f, 0.1f, 1.0f};
constexpr float UI_LETTERBOX_BORDER_WIDTH = 4.0f;

// @menu
extern void UpdateMenu();
extern void InitMenu();
extern void OpenMainMenu();
extern void OpenPauseMenu();
extern Color GetButtonBackgroundColor(ElementState state, float time, void* user_data);

// @edit
extern void InitEditor();
extern void ShutdownEditor();
extern void OpenEditor();
extern void DrawEditor();
extern void UpdateEditorUI();
extern void UpdateEditor();

// @battle
extern void OpenBattle(const BattleSetup& setup);
extern void InitBattle();
extern void ShutdownBattle();
extern void UpdateBattle();
extern void UpdateBattleUI();
extern void DrawBattle();
extern void HandleUnitDeath(UnitEntity* entity, DamageType damage_type);
