//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

constexpr float ARCHER_SPEED = 1.0f;
constexpr float ARCHER_RANGE = 8.0f;
constexpr float ARCHER_COOLDOWN_MIN = 1.4f;
constexpr float ARCHER_COOLDOWN_MAX = 1.6f;
constexpr float ARCHER_DAMAGE = 0.75f;
constexpr float ARCHER_HEALTH = 5.0f;
constexpr float ARCHER_SIZE = .85f;

inline ArcherEntity* CastArcher(Entity* e) {
    assert(e && e->type == ENTITY_TYPE_UNIT);
    ArcherEntity* a = static_cast<ArcherEntity*>(e);
    assert(a->unit_type == UNIT_TYPE_ARCHER);
    return a;
}

static void DrawArcherInternal(Entity* e, const Mat3& transform, bool shadow) {
    DrawStick(e, transform, shadow);
    DrawMesh(MESH_STICK_BOW, transform, e->animator, BONE_STICK_ITEM_B);
}

void DrawArcher(Entity* e, const Mat3& transform) {
    BindDepth(0.0f - (e->position.y) * 5);
    BindMaterial(g_game.material);
    BindTeamColor(static_cast<UnitEntity*>(e)->team);
    DrawArcherInternal(e, transform, false);
}

void DrawArcherShadow(Entity* e, const Mat3& transform) {
    ArcherEntity* a = CastArcher(e);
    DrawArcherInternal(a, transform, true);
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

struct FindArcherTargetArgs {
    ArcherEntity* a;
    UnitEntity* target;
    float target_distance;
};

static bool FindArcherTarget(UnitEntity* u, void* user_data) {
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

    Vec2 avoid = {};
    if (args.target_distance > ARCHER_RANGE) {
        if (a->animator.animation != ANIMATION_STICK_RUN)
            Play(a->animator, ANIMATION_STICK_RUN, 1.0f, true);

        TryGetAvoidVelocity(a, &avoid);

        MoveTowards(a, XY(args.target->position), ARCHER_SPEED, avoid);

        DebugLine(XY(a->position), XY(a->position) + avoid * ARCHER_SPEED, COLOR_RED);
        DebugLine(XY(a->position), XY(a->position) + Normalize(XY(args.target->position) - XY(a->position)) * ARCHER_SPEED, COLOR_GREEN);

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

        // } else if (TryGetAvoidVelocity(a, &avoid)) {
        //     if (a->animator.animation != ANIMATION_STICK_RUN)
        //         Play(a->animator, ANIMATION_STICK_RUN, 1.0f, true);
        //
        //     MoveTowards(a, XY(a->position), ARCHER_SPEED, avoid);
        } else if (!IsPlaying(a->animator) || (a->animator.animation != ANIMATION_ARCHER_IDLE && a->animator.loop)) {
            Play(a->animator, ANIMATION_ARCHER_IDLE, 1.0f, true);
        }
    }

    Update(e->animator, GetGameTimeScale() * 0.5f);
}

ArcherEntity* CreateArcher(Team team, const Vec3& position) {
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
    Play(a->animator, ANIMATION_ARCHER_IDLE, 1.0f, true);
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