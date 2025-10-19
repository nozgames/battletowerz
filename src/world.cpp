//
//  BattleTowerZ - Copyright(c) 2025 NoZ Games, LLC
//

struct WorldSystem {
};

static WorldSystem g_world = {};

static void DrawGrid(Camera* camera) {
    Bounds2 bounds = GetBounds(camera);
    const float left = bounds.min.x;
    const float right = bounds.max.x;
    const float bottom = bounds.min.y;
    const float top = bounds.max.y;

    // Calculate line thickness based on world-to-screen scale
    // Use world units for consistent visual thickness
    Vec2Int screen_size = GetScreenSize();
    float world_height = top - bottom;
    float pixels_per_world_unit = screen_size.y / world_height;
    float line_thickness = 1.0f / pixels_per_world_unit;

    BindDepth(-9.0f);
    BindColor(GRID_COLOR);
    BindMaterial(g_game.material);

    const float start_x = floorf(left);
    for (float x = start_x; x <= right + 1.0f; x += 1.0f) {
        Vec2 line_center = { x, (top + bottom) * 0.5f };
        Vec2 line_scale = { line_thickness, (top - bottom) * 0.5f };
        DrawMesh(g_game.line_mesh, TRS(line_center, 0, line_scale));
    }

    const float start_y = floorf(bottom);
    for (float y = start_y; y <= top + 1.0f; y += 1.0f) {
        Vec2 line_center = { (left + right) * 0.5f, y };
        Vec2 line_scale = { (right - left) * 0.5f, line_thickness };
        DrawMesh(g_game.line_mesh, TRS(line_center, 0, line_scale));
    }

    BindDepth(0.0f);
}

void DrawWorld(Camera* camera) {
    DrawGrid(camera);
}

void InitWorld() {
}
