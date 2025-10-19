//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

struct Editor {
    UnitType selected_unit;
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

void UpdateEditorUI() {
    Canvas([] {
        Align({.alignment = ALIGNMENT_BOTTOM_CENTER, .margin=EdgeInsetsBottom(20)}, [] {
            Row({.spacing=10}, [] {
                UnitButton(UNIT_TYPE_KNIGHT);
                UnitButton(UNIT_TYPE_ARCHER);
            });
        });
    });
}

void UpdateEditor() {
    if (WasButtonPressed(g_game.input, MOUSE_LEFT)) {
        const UnitInfo* unit_info = GetUnitInfo(g_editor.selected_unit);
        if (unit_info) {
            unit_info->create_func(TEAM_RED, g_game.mouse_world_position);
        }
    }
}

void InitEditor() {
    g_editor.selected_unit = UNIT_TYPE_UNKNOWN;
}
