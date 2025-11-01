//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

constexpr float ARCHER_SPEED = 1.0f;
constexpr float ARCHER_RANGE = 8.0f;
constexpr float ARCHER_COOLDOWN_MIN = 1.4f;
constexpr float ARCHER_COOLDOWN_MAX = 1.6f;
constexpr float ARCHER_DAMAGE = 0.75f;
constexpr float ARCHER_HEALTH = 5.0f;
constexpr float ARCHER_SIZE = 0.5f;

inline ArcherEntity* CastArcher(Entity* e) {
    assert(e && e->type == ENTITY_TYPE_UNIT);
    ArcherEntity* a = static_cast<ArcherEntity*>(e);
    assert(a->unit_type == UNIT_TYPE_ARCHER);
    return a;
}

static void DrawStickRightArm(Entity* e, const Mat3& transform, bool ) {
    DrawMesh(MESH_STICK_ARM_L_R, transform, e->animator, BONE_STICK_ARM_LOWER_B);
    DrawMesh(MESH_STICK_ARM_U_R, transform, e->animator, BONE_STICK_ARM_UPPER_B);
    DrawMesh(MESH_STICK_REVOLVER, transform, e->animator, BONE_STICK_ITEM_B);
    DrawMesh(MESH_STICK_HAND_R, transform, e->animator, BONE_STICK_HAND_B);
}

static void DrawStickLeftArm(Entity* e, const Mat3& transform, bool ) {
    DrawMesh(MESH_STICK_ARM_L_L, transform, e->animator, BONE_STICK_ARM_LOWER_F);
    DrawMesh(MESH_STICK_ARM_U_L, transform, e->animator, BONE_STICK_ARM_UPPER_F);
    DrawMesh(MESH_STICK_HAND_L, transform, e->animator, BONE_STICK_HAND_F);
}

static void DrawStickBackLeg(Entity* e, const Mat3& transform, bool) {
    DrawMesh(MESH_STICK_LEG_U_R, transform, e->animator, BONE_STICK_LEG_UPPER_B);
    DrawMesh(MESH_STICK_LEG_L_R, transform, e->animator, BONE_STICK_LEG_LOWER_B);
}

static void DrawStickEyes(UnitEntity* e, const Mat3& transform, bool) {
    DrawMesh(e->health <= 0 ? MESH_STICK_EYE_DEAD : MESH_STICK_EYE, transform, e->animator, BONE_STICK_EYE_F);
    DrawMesh(e->health <= 0 ? MESH_STICK_EYE_DEAD : MESH_STICK_EYE, transform, e->animator, BONE_STICK_EYE_B);
}

static void DrawStickBody(Entity* e, const Mat3& transform, bool ) {
    DrawMesh(MESH_STICK_HIP, transform, e->animator, BONE_STICK_HIP);
    DrawMesh(MESH_STICK_BODY_B, transform, e->animator, BONE_STICK_SPINE);
    DrawMesh(MESH_STICK_BODY, transform, e->animator, BONE_STICK_CHEST);
    DrawMesh(MESH_STICK_LEG_L_L, transform, e->animator, BONE_STICK_LEG_LOWER_F);
    DrawMesh(MESH_STICK_LEG_U_L, transform, e->animator, BONE_STICK_LEG_UPPER_F);
    DrawMesh(MESH_STICK_NECK, transform, e->animator, BONE_STICK_NECK);
    DrawMesh(MESH_STICK_HEAD, transform, e->animator, BONE_STICK_HEAD);
}

static void DrawArcherAttachments(Entity* e, const Mat3& transform, bool) {
    DrawMesh(MESH_STICK_HAT_COWBOY, transform, e->animator, BONE_STICK_HAT);
}

static void BindColor(UnitEntity* u) {
    BindColor(COLOR_WHITE, GetTeamColorOffset(u->team));
}

static void BindOutlineColor(UnitEntity* u) {
    BindColor(COLOR_WHITE, GetTeamColorOffset(u->team) + Vec2{0.0f, 1.0f/16.0f});
}

static void DrawArcherInternal(Entity* e, const Mat3& transform, bool shadow) {
    ArcherEntity* a = CastArcher(e);

    if (!shadow) {
        BindOutlineColor(a);
        DrawStickRightArm(a, transform, shadow);
        BindColor(a);
    }

    DrawStickRightArm(a, transform, shadow);

    if (!shadow) {
        BindOutlineColor(a);
        DrawStickBackLeg(a, transform, shadow);
        BindColor(a);
    }
    DrawStickBackLeg(e, transform, shadow);

    if (!shadow) {
        BindOutlineColor(a);
        DrawStickBody(a, transform, shadow);
        BindColor(a);
    }
    DrawStickBody(a, transform, shadow);

    if (!shadow) {
        BindOutlineColor(a);
        DrawStickLeftArm(a, transform, shadow);
        BindColor(a);
    }
    DrawStickLeftArm(a, transform, shadow);

    if (!shadow) {
        BindColor(a);
    }

    DrawStickEyes(a, transform, false);

    if (!shadow) {
        BindOutlineColor(a);
        DrawArcherAttachments(a, transform, shadow);
        BindColor(a);
    }
    DrawArcherAttachments(a, transform, shadow);
}

void DrawArcher(Entity* e, const Mat3& transform) {
    BindMaterial(g_game.material);
    DrawArcherInternal(e, transform, false);
}

static void UpdateArcherDead(Entity* e) {
    ArcherEntity* a = CastArcher(e);
    Update(a->animator, GetGameTimeScale() * 0.5f);
}

static void KillArcher(Entity* e, DamageType damage_type) {
    UnitEntity* u = static_cast<UnitEntity*>(e);
    u->vtable.update = UpdateArcherDead;
    HandleUnitDeath(u, damage_type);
    //Play(u->animator, ANIMATION_STICK_DEATH, 0.5f, false);
    UpdateArcherDead(e);
    // Free(e);
}


void DrawArcherShadow(Entity* e, const Mat3& transform) {
    ArcherEntity* a = CastArcher(e);
    DrawArcherInternal(a, transform, true);
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

    if (u->health <= 0.0f)
        return true;

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

    if (args.target_distance > ARCHER_RANGE) {
        if (a->animator.animation != ANIMATION_STICK_RUN)
            Play(a->animator, ANIMATION_STICK_RUN, 1.0f, true);

        MoveTowards(a, XY(args.target->position), ARCHER_SPEED);
        a->cooldown = RandomFloat(ARCHER_COOLDOWN_MIN, ARCHER_COOLDOWN_MAX);
    } else {
        a->cooldown -= GetGameFrameTime();
        if (a->cooldown <= 0.0f) {
            a->cooldown = RandomFloat(ARCHER_COOLDOWN_MIN, ARCHER_COOLDOWN_MAX);
            //Play(a->animator, ANIMATION_STICK_BOW_DRAW, 1.0f, false);
            Damage(args.target, DAMAGE_TYPE_PHYSICAL, ARCHER_DAMAGE);
            Play(VFX_ARROW_HIT, WorldToScreen(args.target->position));
            Vec2 hand = TRS(XY(a->position), 0.0f, a->scale) * a->animator.bones[BONE_STICK_HAND_B] * VEC2_ZERO;
            CreateArrow(
                a->team,
                Vec3{hand.x, hand.y, 0.0f},
                XY(args.target->position),
                4.0f);

        } else if (!IsPlaying(a->animator) || (a->animator.animation != ANIMATION_STICK_IDLE && a->animator.loop)) {
            Play(a->animator, ANIMATION_STICK_IDLE, 1.0f, true);
        }
    }

    Update(e->animator, GetGameTimeScale() * 0.5f);
}

ArcherEntity* CreateArcher(Team team, const Vec3& position)
{
    static EntityVtable vtable = {
        .update = UpdateArcher,
        .draw = DrawArcher,
        .draw_shadow = DrawArcherShadow,
        .death = KillArcher
    };

    ArcherEntity* a = static_cast<ArcherEntity*>(CreateUnit(UNIT_TYPE_ARCHER, team, vtable, position, 0.0f, {GetTeamDirection(team).x, 1.0f}));
    a->health = ARCHER_HEALTH;
    a->size = ARCHER_SIZE;
    a->cooldown = RandomFloat(ARCHER_COOLDOWN_MIN, ARCHER_COOLDOWN_MAX);

    Init(a->animator, SKELETON_STICK);
    Play(a->animator, ANIMATION_STICK_IDLE, 1.0f, true);
    return a;
}

void InitArcherUnit() {
    InitUnitInfo({
        .type = UNIT_TYPE_ARCHER,
        .name = GetName("Archer"),
        .size = ARCHER_SIZE,
        .create_func = (UnitCreateFunc)CreateArcher,
        .icon_mesh = MESH_COWBOY_ICON
    });
}