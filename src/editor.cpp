//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

struct EditorUnit {
    const UnitInfo* unit_info;
    Team team;
    Vec2 position;
    UnitEntity* entity;
};

struct Editor {
    UnitType selected_unit;
    EditorUnit* hovered_unit;
    EditorUnit units[MAX_UNITS];
    int unit_count;
};

static Editor g_editor = {};

static void HandleTapUnit(const UnitInfo* unit_info) {
    g_editor.selected_unit = unit_info->type;
}

static Color GetUnitButtonColor(ElementState state, auto, void* user_data) {
    const UnitInfo* unit_info = static_cast<const UnitInfo*>(user_data);
    return g_editor.selected_unit == unit_info->type || (state&ELEMENT_STATE_HOVERED)
        ? HOVER_COLOR
        : FOREGROUND_COLOR;
}

static void UnitButton(UnitType unit_type) {
    const UnitInfo* unit_info = GetUnitInfo(unit_type);
    GestureDetector({.on_tap = [](const TapDetails&, void* user_data) {
        HandleTapUnit(static_cast<const UnitInfo*>(user_data));
    }, .user_data = (void*)unit_info}, [unit_info] {
        Container({.width=80, .height=80}, [unit_info] {
            Rectangle({.color_func = GetUnitButtonColor, .color_func_user_data = (void*)unit_info });
            Container({.margin=EdgeInsetsBottom(10)}, [unit_info] {
                Label(unit_info->name->value, {.font=FONT_SEGUISB, .font_size=14, .color=COLOR_WHITE, .align=ALIGNMENT_BOTTOM_CENTER});
            });
        });
    });
}

void DrawEditor() {
    if (!IsGameState(GAME_STATE_EDIT))
        return;

    BindDepth(-8.0f);
    BindColor(COLOR_RED);
    DrawMesh(g_game.line_mesh, TRS(Vec2{ 0.1f, 0}, 0, Vec2{0.1f, 100}));
    BindColor(COLOR_BLUE);
    DrawMesh(g_game.line_mesh, TRS(Vec2{-0.1f, 0}, 0, Vec2{0.1f, 100}));
    BindDepth(0.0f);
}

static void HandleFightButton(const TapDetails&, void*) {
    g_game.battle_setup.unit_count = 0;
    for (int i = 0; i < g_editor.unit_count; ++i) {
        const EditorUnit& editor_unit = g_editor.units[i];
        g_game.battle_setup.units[g_game.battle_setup.unit_count++] = {
            .unit_info = editor_unit.unit_info,
            .position = editor_unit.entity->position,
            .team = editor_unit.team,
        };
    }

    StartBattle(g_game.battle_setup);;
}

void UpdateEditorUI() {
    if (!IsGameState(GAME_STATE_EDIT))
        return;

    Canvas([] {
        // top
        Align ({.alignment = ALIGNMENT_TOP_CENTER, .margin=EdgeInsetsTop(20)}, [] {
            GestureDetector({.on_tap = HandleFightButton}, [] {
                Container({.width=100, .height=100}, [] {
                     Rectangle({.color_func=GetButtonBackgroundColor});
                     Container({.padding=EdgeInsetsAll(20)}, [] {
                         Image(g_game.material, MESH_ICON_FIGHT, {.color=COLOR_WHITE});
                     });
                 });
            });
        });

        // bottom
        Align({.alignment = ALIGNMENT_BOTTOM_CENTER, .margin=EdgeInsetsBottom(20)}, [] {
            Row({.spacing=10}, [] {
                UnitButton(UNIT_TYPE_TOWER);
                UnitButton(UNIT_TYPE_KNIGHT);
                UnitButton(UNIT_TYPE_ARCHER);
            });
        });
    });
}

static void AddUnit(const UnitInfo* unit_info, Team team, const Vec2& position) {
    if (!unit_info)
        return;

    g_editor.units[g_editor.unit_count++] = {
        .unit_info = unit_info,
        .team = team,
        .position = position,
        .entity = unit_info->create_func(team, position)
    };
}

static int GetEditorUnitIndex(EditorUnit* editor_unit) {
    if (!editor_unit)
        return -1;

    return (int)(editor_unit - &g_editor.units[0]);
}

static EditorUnit* GetEditorUnit(UnitEntity* entity) {
    if (!entity)
        return nullptr;

    for (int i=0; i<g_editor.unit_count; ++i) {
        if (g_editor.units[i].entity == entity)
            return &g_editor.units[i];
    }
    return nullptr;
}

void UpdateEditor() {
    if (!IsGameState(GAME_STATE_EDIT))
        return;

    if (WasButtonPressed(g_game.input, MOUSE_LEFT)) {
        const UnitInfo* unit_info = GetUnitInfo(g_editor.selected_unit);
        AddUnit(
            unit_info,
            g_game.mouse_world_position.x < 0.0f ? TEAM_BLUE : TEAM_RED,
            g_game.mouse_world_position);
    }

    g_editor.hovered_unit = GetEditorUnit(FindClosestUnit(g_game.mouse_world_position));
    if (g_editor.hovered_unit) {
        float distance = Distance(g_editor.hovered_unit->entity->position, g_game.mouse_world_position);
        LogInfo("%s hovered at distance %.2f", g_editor.hovered_unit->unit_info->name->value, distance);
        if (distance > g_editor.hovered_unit->entity->size)
            g_editor.hovered_unit = nullptr;
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

    if (WasButtonReleased(g_game.input, MOUSE_RIGHT)) {
        if (g_editor.hovered_unit) {
            int unit_index = GetEditorUnitIndex(g_editor.hovered_unit);
            assert(unit_index >= 0 && unit_index < g_editor.unit_count);
            Free(g_editor.units[unit_index].entity);
            g_editor.units[unit_index] = g_editor.units[--g_editor.unit_count];
            g_editor.hovered_unit = nullptr;
        }
    }
}

void InitEditor() {
    g_editor = {};
    g_editor.selected_unit = UNIT_TYPE_UNKNOWN;

    DestroyAllEntities();

    for (int i=0; i<g_game.battle_setup.unit_count; ++i) {
        const UnitSetup& unit_setup = g_game.battle_setup.units[i];
        AddUnit(
            unit_setup.unit_info,
            unit_setup.team,
            unit_setup.position);
    }
}
