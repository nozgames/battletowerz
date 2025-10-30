//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

constexpr float BATTLE_SLOW_MOTION_TIME_SCALE = 0.1f;
constexpr float GAME_OVER_FREEZE_TIME = 1.5f;
constexpr float GAME_OVER_UI_TIME = 1.0f;
constexpr float GAME_OVER_LETTERBOX_HEIGHT = 200.0f;
constexpr int GAME_OVER_VICTORY_FONT_SIZE = 60;
constexpr int GAME_OVER_INSTRUCTIONS_FONT_SIZE = 40;

enum BattleState {
    BATTLE_STATE_SIMULATE,
    BATTLE_STATE_GAME_OVER,
};

struct Battle {
    BattleState state;
    int team_counts[TEAM_COUNT];
    bool finished;
    int winning_team;
    float state_time;
    InputSet* input;
};

static Battle g_battle = {};


bool UpdateEntity(u32 index, void* item, void* user_data) {
    (void)index;
    (void)user_data;

    Entity* e = static_cast<Entity*>(item);
    if (e->vtable.update)
        e->vtable.update(e);

    return true;
}

static void UpdateGameOverState() {
    if (WasButtonPressed(g_battle.input, KEY_TAB)) {
        SetGameState(GAME_STATE_EDIT);
        return;
    }

    g_battle.state_time += GetFrameTime();

    float freeze_time = Tween(1.0f, 0.0f, g_battle.state_time, GAME_OVER_FREEZE_TIME, EaseOutQuadratic);
    SetGameTimeScale(BATTLE_SLOW_MOTION_TIME_SCALE * freeze_time);

    Canvas([] {
        float ui_time = Tween(1.0f, 0.0f, g_battle.state_time, GAME_OVER_UI_TIME, EaseOutQuadratic);
        Transformed({.translate = Vec2{0, ui_time * -GAME_OVER_LETTERBOX_HEIGHT}}, [] {
            Align({.alignment = ALIGNMENT_TOP}, [] {
                Container({.height=GAME_OVER_LETTERBOX_HEIGHT, .color = UI_LETTERBOX_COLOR}, [] {
                    Align({.alignment = ALIGNMENT_BOTTOM}, [] {
                        Container({.height=UI_LETTERBOX_BORDER_WIDTH, .color = UI_LETTERBOX_BORDER_COLOR});
                    });

                    if (g_battle.winning_team == TEAM_UNKNOWN) {
                        Label("DRAW!", {.font = FONT_SEGUISB, .font_size = GAME_OVER_VICTORY_FONT_SIZE, .align = ALIGNMENT_CENTER});
                        return;
                    }

                    Align({.alignment = ALIGNMENT_CENTER}, [] {
                        Row([] {
                            if (g_battle.winning_team == TEAM_RED)
                                Label("RED ", {.font = FONT_SEGUISB, .font_size = GAME_OVER_VICTORY_FONT_SIZE, .color = GetTeamColor(TEAM_RED)});
                            else
                                Label("BLUE ", {.font = FONT_SEGUISB, .font_size = GAME_OVER_VICTORY_FONT_SIZE, .color = GetTeamColor(TEAM_BLUE)});

                            Label("VICTORY!", {.font = FONT_SEGUISB, .font_size = GAME_OVER_VICTORY_FONT_SIZE});
                        });
                    });
                });
            });
        });

        Transformed({.translate = Vec2{0, ui_time * GAME_OVER_LETTERBOX_HEIGHT}}, [] {
            Align({.alignment = ALIGNMENT_BOTTOM}, [] {
                Container({.height=GAME_OVER_LETTERBOX_HEIGHT, .color = UI_LETTERBOX_COLOR}, [] {
                    Align({.alignment = ALIGNMENT_TOP}, [] {
                        Container({.height=UI_LETTERBOX_BORDER_WIDTH, .color = UI_LETTERBOX_BORDER_COLOR});
                    });

                    Align({.alignment = ALIGNMENT_CENTER}, [] {
                        Row([] {
                            Label("PRESS ", {.font = FONT_SEGUISB, .font_size = GAME_OVER_INSTRUCTIONS_FONT_SIZE});
                            Container({.padding=EdgeInsets(0,10,0,10), .color = COLOR_WHITE}, [] {
                                Label("TAB ", {.font = FONT_SEGUISB, .font_size = GAME_OVER_INSTRUCTIONS_FONT_SIZE, .color = UI_LETTERBOX_COLOR});
                            });
                            Label(" TO CONTINUE", {.font = FONT_SEGUISB, .font_size = GAME_OVER_INSTRUCTIONS_FONT_SIZE});
                        });
                    });
                });
            });
        });
    });
}

void UpdateBattleUI() {
    if (!IsGameState(GAME_STATE_BATTLE))
        return;

    if (g_battle.state == BATTLE_STATE_GAME_OVER)
        UpdateGameOverState();
}

static void CheckForWinner() {
    int team_count = 0;
    Team winner = TEAM_UNKNOWN;
    for (int i = 0; i < TEAM_COUNT; ++i) {
        if (g_battle.team_counts[i] > 0) {
            winner = static_cast<Team>(i);
            team_count++;
        }
    }

    if (team_count > 1)
        return;

    g_battle.state = BATTLE_STATE_GAME_OVER;
    g_battle.state_time = 0.0f;
    g_battle.winning_team = winner;
}

void UpdateBattle() {
    if (!IsGameState(GAME_STATE_BATTLE))
        return;

    if (g_battle.state == BATTLE_STATE_SIMULATE)
        CheckForWinner();

    UpdateCameraZoom();
    UpdateCameraPan();
    Enumerate(g_game.entity_allocator, UpdateEntity);
}

void DrawBattle() {
    if (!IsGameState(GAME_STATE_BATTLE))
        return;

    DrawGrid(g_game.camera);
}

void HandleUnitDeath(UnitEntity* entity, DamageType damage_type) {
    (void) damage_type;
    g_battle.team_counts[entity->team]--;
}

void ShutdownBattle() {
    PopInputSet();
    Free(g_battle.input);
    g_battle = {};
}

void OpenBattle(const BattleSetup& setup) {
    g_game.battle_setup = setup;
    SetGameState(GAME_STATE_BATTLE);
}

void InitBattle() {
    g_battle = {};
    g_battle.state = BATTLE_STATE_SIMULATE;
    g_battle.input = CreateInputSet(ALLOCATOR_DEFAULT);
    EnableButton(g_battle.input, MOUSE_LEFT);
    EnableButton(g_battle.input, MOUSE_RIGHT);
    EnableButton(g_battle.input, KEY_ESCAPE);
    EnableButton(g_battle.input, KEY_TAB);
    PushInputSet(g_battle.input);

    SetGameTimeScale(1.0f);

    DestroyAllEntities();

    for (int i = 0; i < g_game.battle_setup.unit_count; ++i) {
        const UnitSetup& unit_setup = g_game.battle_setup.units[i];
        g_battle.team_counts[unit_setup.team]++;
        unit_setup.unit_info->create_func(
            unit_setup.team,
            unit_setup.position);
    }

    ResetCamera();
}
