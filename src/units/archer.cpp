//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#include "game.h"

constexpr float ARCHER_SPEED = 5.0f;
constexpr float ARCHER_RANGE = 8.0f;
constexpr float ARCHER_COOLDOWN = 1.5f;
constexpr float ARCHER_DAMAGE = 0.75f;
constexpr float ARCHER_HEALTH = 5.0f;
constexpr float ARCHER_SIZE = 0.25f;

inline ArcherEntity* CastArcher(Entity* e) {
    assert(e && e->type == ENTITY_TYPE_UNIT);
    ArcherEntity* a = static_cast<ArcherEntity*>(e);
    assert(a->unit_type == UNIT_TYPE_ARCHER);
    return a;
}

void RenderArcher(Entity* e, const Mat3& transform)
{
    ArcherEntity* a = CastArcher(e);
    BindColor(GetTeamColor(a->team));
    BindTransform(transform);
    BindMaterial(g_game.material);
    DrawMesh(MESH_UNIT_ARCHER);
}

struct FindArcherTargetArgs {
    ArcherEntity* a;
    UnitEntity* target;
    float target_distance;
};

static bool FindArcherTarget(UnitEntity* u, void* user_data)
{
    assert(u);
    assert(user_data);
    FindArcherTargetArgs* args = static_cast<FindArcherTargetArgs*>(user_data);
    float distance = Distance(args->a->position, u->position);
    if (distance < args->target_distance) {
        args->target = u;
        args->target_distance = distance;
    }
    return true;
}

void UpdateArcher(Entity* e)
{
    ArcherEntity* a = CastArcher(e);
    FindArcherTargetArgs args {
        .a = a,
        .target = nullptr,
        .target_distance = F32_MAX
    };
    EnumerateUnits(GetOppositeTeam(a->team), FindArcherTarget, &args);
    if (!args.target)
        return;

    if (args.target_distance > ARCHER_RANGE) {
        e->position += GetTeamDirection(a->team) * GetFrameTime() * ARCHER_SPEED;
        a->cooldown = ARCHER_COOLDOWN;
    } else {
        a->cooldown -= GetFrameTime();
        if (a->cooldown <= 0.0f) {
            a->cooldown = ARCHER_COOLDOWN;
            Damage(args.target, DAMAGE_TYPE_PHYSICAL, ARCHER_DAMAGE);
            Play(VFX_ARROW_HIT, args.target->position);
        }
    }
}

ArcherEntity* CreateArcher(Team team, const Vec2& position)
{
    static EntityVtable vtable = {
        .update = UpdateArcher,
        .render = RenderArcher
    };

    ArcherEntity* a = static_cast<ArcherEntity*>(CreateUnit(UNIT_TYPE_ARCHER, team, vtable, position, 0.0f, {GetTeamDirection(team).x, 1.0f}));
    a->health = ARCHER_HEALTH;
    a->size = ARCHER_SIZE;
    return a;
}