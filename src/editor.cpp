//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

constexpr float EDITOR_UI_BOTTOM_HEIGHT = 120.0f + UI_LETTERBOX_BORDER_WIDTH;

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
    InputSet* input;
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

void DrawEditor() {
    if (!IsGameState(GAME_STATE_EDIT))
        return;

    BindDepth(-9.5f);
    BindMaterial(g_game.material);
    BindColor(SetAlpha(COLOR_WHITE, 0.9f), GetTeamColorOffset(TEAM_RED));
    for (int y=-20; y<=20; ++y)
        DrawMesh(MESH_TEAM_LINE, TRS(Vec2{0, (float)y }, 0, Vec2{-1, 1}));
    BindColor(SetAlpha(COLOR_WHITE, 0.9f), GetTeamColorOffset(TEAM_BLUE));
    for (int y=-20; y<=20; ++y)
        DrawMesh(MESH_TEAM_LINE, TRS(Vec2{0, (float)y }, 0, Vec2{1, 1}));

    BindDepth(0.0f);
    DrawGrid(g_game.camera);
}

static void BeginBattle() {
    g_game.battle_setup.unit_count = 0;
    for (int i = 0; i < g_editor.unit_count; ++i) {
        const EditorUnit& editor_unit = g_editor.units[i];
        g_game.battle_setup.units[g_game.battle_setup.unit_count++] = {
            .unit_info = editor_unit.unit_info,
            .position = XY(editor_unit.entity->position),
            .team = editor_unit.team,
        };
    }

    OpenBattle(g_game.battle_setup);
}

static void UnitButton(UnitType unit_type) {
    const UnitInfo* unit_info = GetUnitInfo(unit_type);
    GestureDetector({.on_tap = [](const TapDetails&, void* user_data) {
        HandleTapUnit(static_cast<const UnitInfo*>(user_data));
    }, .user_data = (void*)unit_info}, [unit_info] {
        Container({.width=120, .height=120}, [unit_info] {
            Rectangle({.color_func = GetUnitButtonColor, .color_func_user_data = (void*)unit_info });
            Container({.margin=EdgeInsetsBottom(10)}, [unit_info] {
                Align({.alignment=ALIGNMENT_TOP_CENTER, .margin = EdgeInsetsTop(10)}, [unit_info] {
                    SizedBox({.width=70, .height=70}, [unit_info] {
                        Image(g_game.material, unit_info->icon_mesh, {.color=COLOR_WHITE});
                    });
                });
                Label(unit_info->name->value, {.font=FONT_SEGUISB, .font_size=18, .color=COLOR_WHITE, .align=ALIGNMENT_BOTTOM_CENTER});
            });
        });
    });
}

void UpdateEditorUI() {
    if (!IsGameState(GAME_STATE_EDIT))
        return;

    Canvas([] {
        // top
        Align ({.alignment = ALIGNMENT_TOP_CENTER, .margin=EdgeInsetsTop(20)}, [] {
            GestureDetector({.on_tap = [] (auto,auto) { BeginBattle(); }}, [] {
                Container({.width=100, .height=100}, [] {
                     Rectangle({.color_func=GetButtonBackgroundColor});
                     Container({.padding=EdgeInsetsAll(20)}, [] {
                         Image(g_game.material, MESH_ICON_FIGHT, {.color=COLOR_WHITE});
                     });
                 });
            });
        });

        // bottom
        Align({.alignment = ALIGNMENT_BOTTOM}, [] {
            Container({.height = EDITOR_UI_BOTTOM_HEIGHT, .color = UI_LETTERBOX_COLOR}, [] {
                Align({.alignment = ALIGNMENT_TOP}, [] {
                    Container({.height=UI_LETTERBOX_BORDER_WIDTH, .color = UI_LETTERBOX_BORDER_COLOR});
                });
                Align({.alignment = ALIGNMENT_BOTTOM_CENTER}, [] {
                    Row({.spacing=10}, [] {
                        // UnitButton(UNIT_TYPE_TOWER);
                        // UnitButton(UNIT_TYPE_KNIGHT);
                        // UnitButton(UNIT_TYPE_ARCHER);
                        UnitButton(UNIT_TYPE_COWBOY);
                        UnitButton(UNIT_TYPE_ARCHER);
                    });
                });
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

static bool TryPlaceUnit(const Vec2& position, float size) {
    UnitEntity* u = FindClosestUnit(position);
    if (!u)
        return true;

    float distance = Distance(XY(u->position), position);
    return distance - size > 0.0f;
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

    if (WasButtonPressed(g_editor.input, KEY_TAB)) {
        BeginBattle();
        return;
    }

    if (WasButtonPressed(g_editor.input, KEY_ESCAPE)) {
        ShutdownEditor();
        OpenMainMenu();
        return;
    }

    if (IsButtonDown(g_editor.input, MOUSE_LEFT)) {
        const UnitInfo* unit_info = GetUnitInfo(g_editor.selected_unit);
        if (unit_info && TryPlaceUnit(g_game.mouse_world_position, unit_info->size))
            AddUnit(
                unit_info,
                g_game.mouse_world_position.x < 0.0f ? TEAM_BLUE : TEAM_RED,
                g_game.mouse_world_position);
    }

    g_editor.hovered_unit = GetEditorUnit(FindClosestUnit(g_game.mouse_world_position));
    if (g_editor.hovered_unit) {
        float distance = Distance(XY(g_editor.hovered_unit->entity->position), g_game.mouse_world_position);
        if (distance > g_editor.hovered_unit->entity->size)
            g_editor.hovered_unit = nullptr;
    }

    UpdateCameraPan();
    UpdateCameraZoom();

    if (WasButtonReleased(g_editor.input, MOUSE_RIGHT)) {
        if (g_editor.hovered_unit) {
            int unit_index = GetEditorUnitIndex(g_editor.hovered_unit);
            assert(unit_index >= 0 && unit_index < g_editor.unit_count);
            Free(g_editor.units[unit_index].entity);
            g_editor.units[unit_index] = g_editor.units[--g_editor.unit_count];
            g_editor.hovered_unit = nullptr;
        }
    }
}

void ShutdownEditor() {
    PopInputSet();
    DestroyAllEntities();
    Free(g_editor.input);
    g_editor = {};
}

void InitEditor() {
    g_editor.selected_unit = UNIT_TYPE_UNKNOWN;

    g_editor.input = CreateInputSet(ALLOCATOR_DEFAULT);
    EnableButton(g_editor.input, KEY_TAB);
    EnableButton(g_editor.input, KEY_ESCAPE);
    EnableButton(g_editor.input, MOUSE_LEFT);
    EnableButton(g_editor.input, MOUSE_RIGHT);

    ResetCamera();
    DestroyAllEntities();
    SetGameTimeScale(0.0f);
    PushInputSet(g_editor.input);

    for (int i=0; i<g_game.battle_setup.unit_count; ++i) {
        const UnitSetup& unit_setup = g_game.battle_setup.units[i];
        AddUnit(
            unit_setup.unit_info,
            unit_setup.team,
            unit_setup.position);
    }

    SetGameState(GAME_STATE_EDIT);
}
