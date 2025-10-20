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

void RenderArcher(Entity* e, const Mat3& transform_a) {
    ArcherEntity* a = CastArcher(e);
    BindColor(GetTeamColor(a->team));
    BindMaterial(g_game.material);

    Mat3 transform = transform_a * Scale(1);

    BindDepth(2.0f - (a->position.y / 10.0f));
    DrawMesh(MESH_HUMAN_FOOT_R, transform, e->animator, BONE_ARCHER_FOOT_R);
    DrawMesh(MESH_HUMAN_LEG_L, transform, e->animator, BONE_ARCHER_LEG_L);
    DrawMesh(MESH_HUMAN_LEG_R, transform, e->animator, BONE_ARCHER_LEG_R);
    DrawMesh(MESH_HUMAN_FOOT_L, transform, e->animator, BONE_ARCHER_FOOT_L);
    DrawMesh(MESH_ARCHER_BODY, transform, e->animator, BONE_ARCHER_BODY);
    DrawMesh(MESH_ARCHER_HEAD, transform, e->animator, BONE_ARCHER_HEAD);
    DrawMesh(MESH_HUMAN_EYE, transform, e->animator, BONE_ARCHER_EYE_L);
    DrawMesh(MESH_HUMAN_EYE, transform, e->animator, BONE_ARCHER_EYE_R);
    DrawMesh(MESH_WEAPON_BOW, transform, e->animator, BONE_ARCHER_HAND_R);
    DrawMesh(MESH_HUMAN_HAND, transform, e->animator, BONE_ARCHER_HAND_R);
    DrawMesh(MESH_HUMAN_HAND, transform, e->animator, BONE_ARCHER_HAND_L);
    BindDepth(0.0f);

    BindDepth(-7.0f);
    transform = transform * Scale({1, -0.5f});
    BindColor({0,0,0,0.1f});
    BindMaterial(g_game.shadow_material);
    DrawMesh(MESH_HUMAN_FOOT_R, transform, e->animator, BONE_ARCHER_FOOT_R);
    DrawMesh(MESH_HUMAN_LEG_L, transform, e->animator, BONE_ARCHER_LEG_L);
    DrawMesh(MESH_HUMAN_LEG_R, transform, e->animator, BONE_ARCHER_LEG_R);
    DrawMesh(MESH_HUMAN_FOOT_L, transform, e->animator, BONE_ARCHER_FOOT_L);
    DrawMesh(MESH_ARCHER_BODY, transform, e->animator, BONE_ARCHER_BODY);
    DrawMesh(MESH_ARCHER_HEAD, transform, e->animator, BONE_ARCHER_HEAD);
    DrawMesh(MESH_HUMAN_EYE, transform, e->animator, BONE_ARCHER_EYE_L);
    DrawMesh(MESH_HUMAN_EYE, transform, e->animator, BONE_ARCHER_EYE_R);
    DrawMesh(MESH_WEAPON_BOW, transform, e->animator, BONE_ARCHER_HAND_R);
    DrawMesh(MESH_HUMAN_HAND, transform, e->animator, BONE_ARCHER_HAND_R);
    DrawMesh(MESH_HUMAN_HAND, transform, e->animator, BONE_ARCHER_HAND_L);
    BindDepth(0.0f);
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
    float distance = Distance(XY(args->a->position), XY(u->position));
    if (distance < args->target_distance) {
        args->target = u;
        args->target_distance = distance;
    }
    return true;
}

void UpdateArcher(Entity* e) {
    ArcherEntity* a = CastArcher(e);
    FindArcherTargetArgs args {
        .a = a,
        .target = nullptr,
        .target_distance = F32_MAX
    };
    EnumerateUnits(GetOppositeTeam(a->team), FindArcherTarget, &args);
    if (!args.target)
        return;

    if (!IsPlaying(a->animator) && !IsLooping(a->animator)) {
        Vec2 hand = TRS(XY(a->position), 0.0f, a->scale) * a->animator.bones[BONE_ARCHER_HAND_R] * VEC2_ZERO;
        Play(a->animator, ANIMATION_ARCHER_IDLE, 1.0f, true);
        CreateArrow(
            a->team,
            Vec3{hand.x, hand.y, 0.0f},
            XY(args.target->position),
            4.0f);
    }

    if (args.target_distance > ARCHER_RANGE) {
        MoveTowards(a, XY(args.target->position), ARCHER_SPEED);
        a->cooldown = ARCHER_COOLDOWN;
    } else {
        a->cooldown -= GetGameFrameTime();
        if (a->cooldown <= 0.0f) {
            a->cooldown = ARCHER_COOLDOWN;
            Play(a->animator, ANIMATION_ARCHER_DRAW, 1.0f, false);
            //Damage(args.target, DAMAGE_TYPE_PHYSICAL, ARCHER_DAMAGE);
            //Play(VFX_ARROW_HIT, WorldToScreen(args.target->position));
        }
    }

    Update(a->animator);
}

ArcherEntity* CreateArcher(Team team, const Vec3& position)
{
    static EntityVtable vtable = {
        .update = UpdateArcher,
        .render = RenderArcher
    };

    ArcherEntity* a = static_cast<ArcherEntity*>(CreateUnit(UNIT_TYPE_ARCHER, team, vtable, position, 0.0f, {GetTeamDirection(team).x, 1.0f}));
    a->health = ARCHER_HEALTH;
    a->size = ARCHER_SIZE;

    Init(a->animator, SKELETON_ARCHER);
    Play(a->animator, ANIMATION_ARCHER_IDLE, 1.0f, true);
    return a;
}