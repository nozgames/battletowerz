//
//  NockerZ - Copyright(c) 2025 NoZ Games, LLC
//

#include "game.h"

constexpr float ARCHER_SPEED = 5.0f;

void RenderArcher(Entity* e, const Mat3& transform)
{
    assert(e);
    assert(e->type == ENTITY_TYPE_ARCHER);
    ArcherEntity* a = static_cast<ArcherEntity*>(e);

    BindColor(GetTeamColor(a->team));
    BindTransform(transform);
    BindMaterial(g_game.material);
    DrawMesh(MESH_UNIT_ARCHER);
}

void UpdateArcher(Entity* e)
{
    assert(e);
    assert(e->type == ENTITY_TYPE_ARCHER);
    ArcherEntity* a = static_cast<ArcherEntity*>(e);
    e->position += GetTeamDirection(a->team) * GetFrameTime() * ARCHER_SPEED;
}

ArcherEntity* CreateArcher(Team team, const Vec2& position)
{
    static EntityVtable vtable = {
        .update = UpdateArcher,
        .render = RenderArcher
    };

    ArcherEntity* archer = static_cast<ArcherEntity*>(CreateEntity(ENTITY_TYPE_ARCHER, vtable, position, 0.0f, {GetTeamDirection(team).x, 1.0f}));
    archer->team = team;
    return archer;
}