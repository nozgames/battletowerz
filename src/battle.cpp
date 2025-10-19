//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

enum BattleState {
    BATTLE_STATE_SIMULATE,
    BATTLE_STATE_GAME_OVER,
};

struct Battle {
    BattleState state;
    int team_counts[TEAM_COUNT];
    bool finished;
    int winning_team;
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

static void UpdateGameOverMenu() {
    Canvas([] {
        Align({.alignment = ALIGNMENT_CENTER}, [] {
            Column({.spacing = 20.0f}, [] {
                Label("Battle Over!", {.font = FONT_SEGUISB, .font_size = 50, .align = ALIGNMENT_CENTER});
                const char* result_text = (g_battle.winning_team == TEAM_UNKNOWN) ? "It's a Draw!" : (g_battle.winning_team == TEAM_RED) ? "Red Wins!" : "Blue Wins!";
                Label(result_text, {.font = FONT_SEGUISB, .font_size = 40, .align = ALIGNMENT_CENTER});
                Container({.width=400, .height = 100}, [] {
                    GestureDetector({.on_tap = [](const TapDetails&, void*) {
                        SetGameState(GAME_STATE_EDIT);
                    }}, [] {
                        Rectangle({.color_func = [](auto s, auto, auto) { return (s&ELEMENT_STATE_HOVERED) ? HOVER_COLOR : FOREGROUND_COLOR; }});
                        Label("RETURN TO MAIN MENU", {.font = FONT_SEGUISB, .font_size = 30, .align = ALIGNMENT_CENTER});
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
        UpdateGameOverMenu();
}

void UpdateBattle() {
    if (!IsGameState(GAME_STATE_BATTLE))
        return;

    if (g_battle.state != BATTLE_STATE_SIMULATE)
        return;

    int team_count = 0;
    Team winner = TEAM_UNKNOWN;
    for (int i = 0; i < TEAM_COUNT; ++i) {
        if (g_battle.team_counts[i] > 0) {
            winner = static_cast<Team>(i);
            team_count++;
        }
    }

    if (team_count <= 1) {
        g_battle.state = BATTLE_STATE_GAME_OVER;
        g_battle.winning_team = winner;
        return;
    }

    if (WasButtonPressed(g_game.input, MOUSE_RIGHT)) {
        g_game.pan_position = g_game.mouse_position;
        g_game.pan_position_camera = GetPosition(g_game.camera);
    }

    if (IsButtonDown(g_game.input, MOUSE_RIGHT)) {
        Vec2 delta = g_game.mouse_position - g_game.pan_position;
        Vec2 world_delta = ScreenToWorld(g_game.camera, delta) - ScreenToWorld(g_game.camera, VEC2_ZERO);
        SetPosition(g_game.camera, g_game.pan_position_camera - world_delta);
    }

    if (IsGameState(GAME_STATE_BATTLE)) {
        Enumerate(g_game.entity_allocator, UpdateEntity);
    }
}

void DrawBattle() {
    if (!IsGameState(GAME_STATE_BATTLE))
        return;
}

void HandleUnitDeath(UnitEntity* entity, DamageType damage_type) {
    (void) damage_type;
    g_battle.team_counts[entity->team]--;
}

void StartBattle(const BattleSetup& setup) {
    g_game.battle_setup = setup;

    g_battle = {};
    g_battle.state = BATTLE_STATE_SIMULATE;

    DestroyAllEntities();

    for (int i = 0; i < g_game.battle_setup.unit_count; ++i) {
        const UnitSetup& unit_setup = g_game.battle_setup.units[i];
        g_battle.team_counts[unit_setup.team]++;
        unit_setup.unit_info->create_func(
            unit_setup.team,
            unit_setup.position);
    }

    SetGameState(GAME_STATE_BATTLE);
}
