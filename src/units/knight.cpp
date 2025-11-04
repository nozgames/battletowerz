//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#include "game.h"

constexpr float KNIGHT_SPEED = 4.0f;
constexpr float KNIGHT_RANGE = 1.0f;
constexpr float KNIGHT_COOLDOWN = 1.5f;
constexpr float KNIGHT_HEALTH = 10.0f;
constexpr float KNIGHT_DAMAGE = 1.5f;

inline KnightEntity* CastKnight(Entity* e) {
    assert(e && e->type == ENTITY_TYPE_UNIT);
    KnightEntity* a = static_cast<KnightEntity*>(e);
    assert(a->unit_type == UNIT_TYPE_KNIGHT);
    return a;
}

static void RenderKnight(Entity* e, const Mat3& transform)
{
    KnightEntity* a = CastKnight(e);
    BindColor(GetTeamColor(a->team));
    BindMaterial(g_game.material);
    // DrawMesh(MESH_WEAPON_SWORD, transform, a->animator, BONE_UNIT_KNIGHT_WEAPON);
    // DrawMesh(MESH_UNIT_KNIGHT, transform, a->animator, BONE_UNIT_KNIGHT_BODY);
    // DrawMesh(MESH_UNIT_KNIGHT_VISOR, transform, a->animator, BONE_UNIT_KNIGHT_VISOR);

    BindDepth(-7.0f);
    BindColor(SetAlpha(COLOR_BLACK, 0.1f));
    Mat3 shadow_transform = transform * Scale(Vec2{1.0f, -0.5f});
    BindMaterial(g_game.shadow_material);
    // DrawMesh(MESH_WEAPON_SWORD, shadow_transform, a->animator, BONE_UNIT_KNIGHT_WEAPON);
    // DrawMesh(MESH_UNIT_KNIGHT, shadow_transform, a->animator, BONE_UNIT_KNIGHT_BODY);
    // DrawMesh(MESH_UNIT_KNIGHT_VISOR, shadow_transform, a->animator, BONE_UNIT_KNIGHT_VISOR);
    BindDepth(0.0f);
}

struct FindKnightTargetArgs {
    KnightEntity* a;
    UnitEntity* target;
    float target_distance;
};

static bool FindKnightTarget(UnitEntity* u, void* user_data)
{
    assert(u);
    assert(user_data);
    FindKnightTargetArgs* args = static_cast<FindKnightTargetArgs*>(user_data);
    float distance = Distance(XY(args->a->position), XY(u->position));
    if (distance < args->target_distance) {
        args->target = u;
        args->target_distance = distance;
    }
    return true;
}

static void SetKnightIdleState(KnightEntity* u) {
    if (u->state == UNIT_STATE_IDLE)
        return;
    u->state = UNIT_STATE_IDLE;
    //Play(u->animator, ANIMATION_UNIT_KNIGHT_IDLE, 1.0f, true);
}

void UpdateArrow(Entity* e)
{
    KnightEntity* u = CastKnight(e);
    Update(u->animator);

    FindKnightTargetArgs args {
        .a = u,
        .target = nullptr,
        .target_distance = F32_MAX
    };
    EnumerateUnits(GetOppositeTeam(u->team), FindKnightTarget, &args);
    if (!args.target) {
        SetKnightIdleState(u);
        return;
    }

    if (u->cooldown > 0.0f) {
        u->cooldown -= GetGameFrameTime();
        if (u->cooldown < 0.0f)
            u->cooldown = 0.0f;
    }

    if (args.target_distance - args.target->size > KNIGHT_RANGE) {
        MoveTowards(u, XY(args.target->position), KNIGHT_SPEED);
        u->cooldown = KNIGHT_COOLDOWN;

        if (u->state != UNIT_STATE_MOVE) {
            u->state = UNIT_STATE_MOVE;
            //Play(u->animator, ANIMATION_UNIT_KNIGHT_RUN, 1.0f, true);
        }

    } else if (u->cooldown <= 0.0f) {
        u->cooldown = KNIGHT_COOLDOWN;
        u->state = UNIT_STATE_ATTACKING;
        Damage(args.target, DAMAGE_TYPE_PHYSICAL, KNIGHT_DAMAGE);
        Play(VFX_ARROW_HIT, WorldToScreen(args.target->position));
        //Play(u->animator, ANIMATION_UNIT_KNIGHT_ATTACK, 1.0f, false);
    } else if (u->state != UNIT_STATE_IDLE && (IsLooping(u->animator) || !IsPlaying(u->animator))) {
        SetKnightIdleState(u);
    }
}

KnightEntity* CreateKnight(Team team, const Vec3& position)
{
    static EntityVtable vtable = {
        .update = UpdateArrow,
        .draw = RenderKnight
    };

    KnightEntity* k = static_cast<KnightEntity*>(CreateUnit(UNIT_TYPE_KNIGHT, team, vtable, position, 0.0f, {GetTeamDirection(team).x, 1.0f}));
    k->health = KNIGHT_HEALTH;
    k->size = 1.0f;
    // Init(k->animator, SKELETON_UNIT_KNIGHT);
    // Play(k->animator, ANIMATION_UNIT_KNIGHT_IDLE, 1.0f, true);
    return k;
}