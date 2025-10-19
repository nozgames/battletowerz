//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

static void UpdateMainMenu() {
    Canvas({}, [] {
        Image(g_game.background_material, g_game.background_mesh, {.color = BACKGROUND_COLOR });
        Align({.alignment = ALIGNMENT_BOTTOM_LEFT, .margin=EdgeInsetsBottomLeft(40)}, [] {
            Column({.spacing = 40.0f}, [] {
                Container({.width=300, .height = 100}, [] {
                    GestureDetector({.on_tap = [](const TapDetails&, void*) {
                        SetGameState(GAME_STATE_PLAYING);
                    }}, [] {
                        Rectangle({.color_func = [](auto s, auto, auto) { return (s&ELEMENT_STATE_HOVERED) ? HOVER_COLOR : FOREGROUND_COLOR; }});
                        Container({.padding=EdgeInsetsLeft(40)}, [] {
                            Label("PLAY", {.font = FONT_SEGUISB, .font_size = 40, .align=ALIGNMENT_CENTER_LEFT });
                        });
                    });
                });
                Container({.width=300, .height = 100}, [] {
                    GestureDetector({.on_tap = [](const TapDetails&, void*) {
                        SetGameState(GAME_STATE_PLAYING);
                    }}, [] {
                        Rectangle({.color_func = [](auto s, auto, auto) { return (s&ELEMENT_STATE_HOVERED) ? HOVER_COLOR : FOREGROUND_COLOR; }});
                        Container({.padding=EdgeInsetsLeft(40)}, [] {
                            Label("QUIT", {.font = FONT_SEGUISB, .font_size = 40, .align=ALIGNMENT_CENTER_LEFT });
                        });
                    });
                });
            });
        });
    });
}

void OpenMainMenu() {
    SetGameState(GAME_STATE_MAIN_MENU);
}

static void UpdatePauseMenu() {
    Canvas([] {
        Image(g_game.background_material, g_game.background_mesh, {.color = BACKGROUND_COLOR });
        Align({.alignment = ALIGNMENT_CENTER}, [] {
            Column({.spacing = 40.0f}, [] {
                Expanded();
                Container({.width=400, .height = 100 }, [] {
                    GestureDetector({.on_tap = [](const TapDetails&, void*) {
                        SetGameState(GAME_STATE_PLAYING);
                    }}, [] {
                        Rectangle({.color_func = [](auto s, auto, auto) { return (s&ELEMENT_STATE_HOVERED) ? HOVER_COLOR : FOREGROUND_COLOR; }});
                        Label("RESUME", {.font = FONT_SEGUISB, .font_size = 40, .align = ALIGNMENT_CENTER});
                    });
                });
                Container({.width=400, .height = 100}, [] {
                    GestureDetector({.on_tap = [](const TapDetails&, void*) {
                        SetGameState(GAME_STATE_PLAYING);
                    }}, [] {
                        Rectangle({.color_func = [](auto s, auto, auto) { return (s&ELEMENT_STATE_HOVERED) ? HOVER_COLOR : FOREGROUND_COLOR; }});
                        Label("QUIT", {.font = FONT_SEGUISB, .font_size = 40, .align = ALIGNMENT_CENTER });
                    });
                });
                Expanded();
            });
        });
    });
}

void OpenPauseMenu() {
    SetGameState(GAME_STATE_PAUSED);
}

void UpdateMenu() {
    if (g_game.state == GAME_STATE_MAIN_MENU)
        UpdateMainMenu();
    else if (g_game.state == GAME_STATE_PAUSED)
        UpdatePauseMenu();
}

void InitMenu() {
}