//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#include "game.h"

constexpr float TOWER_HEALTH = 20.0f;
constexpr float TOWER_SIZE = 1.0f;

inline TowerEntity* CastArcher(Entity* e) {
    assert(e && e->type == ENTITY_TYPE_UNIT);
    TowerEntity* t = static_cast<TowerEntity*>(e);
    assert(t->unit_type == UNIT_TYPE_TOWER);
    return t;
}

static void RenderTower(Entity* e, const Mat3& transform)
{
    TowerEntity* a = CastArcher(e);
    BindColor(GetTeamColor(a->team));
    BindMaterial(g_game.material);
    DrawMesh(MESH_TOWER_PLAYER_TEMP, transform);
    BindDepth(-7.0f);
    BindMaterial(g_game.shadow_material);
    BindColor(SetAlpha(COLOR_BLACK, 0.1f));
    DrawMesh(MESH_TOWER_PLAYER_TEMP, transform * Scale(Vec2{1.0f,-0.5f}));
    BindDepth(0.0f);
}

TowerEntity* CreateTower(Team team, const Vec2& position)
{
    static EntityVtable vtable = {
        .render = RenderTower
    };

    TowerEntity* t = static_cast<TowerEntity*>(CreateUnit(UNIT_TYPE_TOWER, team, vtable, position, 0.0f, {GetTeamDirection(team).x, 1.0f}));
    t->health = TOWER_HEALTH;
    t->size = TOWER_SIZE;
    return t;
}