//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

struct Battle {
    int team_counts[TEAM_COUNT];
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

void UpdateBattle() {
    if (!IsGameState(GAME_STATE_BATTLE))
        return;

    int team_count = 0;
    for (int i = 0; i < TEAM_COUNT; ++i) {
        if (g_battle.team_counts[i] > 0)
            team_count++;
    }

    if (team_count <= 1) {
        SetGameState(GAME_STATE_EDIT);
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

    if (IsGameState(GAME_STATE_BATTLE))
        Enumerate(g_game.entity_allocator, UpdateEntity);
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
    g_battle = {};
    g_game.battle_setup = setup;

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
