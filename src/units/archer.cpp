//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

constexpr float ARCHER_SPEED = 1.0f;
constexpr float ARCHER_RANGE = 8.0f;
constexpr float ARCHER_COOLDOWN_MIN = 1.4f;
constexpr float ARCHER_COOLDOWN_MAX = 1.6f;
constexpr float ARCHER_DAMAGE = 0.75f;
constexpr float ARCHER_HEALTH = 5.0f;
constexpr float ARCHER_SIZE = .2f;
constexpr float ARCHER_HEIGHT = 0.8f;

inline ArcherEntity* CastArcher(Entity* e) {
    assert(e && e->type == ENTITY_TYPE_UNIT);
    ArcherEntity* a = static_cast<ArcherEntity*>(e);
    //assert(a->unit_type == UNIT_TYPE_ARCHER);
    return a;
}

void DrawArcherInternal(Entity* e, const Mat3& transform, bool shadow) {
    ArcherEntity* a = CastArcher(e);
    DrawStick(e, transform, shadow);
    DrawMesh(MESH_STICK_BOW, transform, e->animator, BONE_STICK_ITEM_B);

    if (a->state == UNIT_STATE_RELOAD) {
        DrawMesh(MESH_PROJECTILE_ARROW, transform, e->animator, BONE_STICK_ITEM_F);
    }
}

void DrawArcher(Entity* e, const Mat3& transform) {
    BindDepth(0.0f - (e->position.y) * 5);
    BindMaterial(g_game.material);
    BindTeamColor(static_cast<UnitEntity*>(e)->team);
    DrawArcherInternal(e, transform, false);


    //DrawGizmos(static_cast<UnitEntity*>(e), transform);
}

void DrawArcherShadow(Entity* e, const Mat3& transform) {
    ArcherEntity* a = CastArcher(e);
    DrawArcherInternal(a, transform, true);
}

static void UpdateArcherDead(Entity*) {
    ///ArcherEntity* a = CastArcher(e);
    //Update(a->animator, GetGameTimeScale() * 0.5f);
}

static void KillArcher(Entity* e, DamageType damage_type) {
    UnitEntity* u = static_cast<UnitEntity*>(e);
    u->vtable.update = UpdateArcherDead;
    HandleUnitDeath(u, damage_type);
    //Play(u->animator, ANIMATION_STICK_DEATH, 0.5f, false);
    UpdateArcherDead(e);
     Free(e);
}

void UpdateArcher(Entity* e) {
    ArcherEntity* a = CastArcher(e);

#if 0
    // Face the target if we have one
    if (a->target) {
        Vec2 to_target = XY(a->target->position) - XY(a->position);
        if (to_target.x > 0.0f) {
            a->scale.x = fabsf(a->scale.x); // Face right
        } else if (to_target.x < 0.0f) {
            a->scale.x = -fabsf(a->scale.x); // Face left
        }
    }

    // Handle attacking when in attacking state
    if (a->state == UNIT_STATE_ATTACK && a->target) {
        a->cooldown -= GetGameFrameTime();
        if (a->cooldown <= 0.0f) {
            a->cooldown = RandomFloat(ARCHER_COOLDOWN_MIN, ARCHER_COOLDOWN_MAX);
            //Play(a->animator, ANIMATION_STICK_BOW_DRAW, 1.0f, false);
            Damage(a->target, DAMAGE_TYPE_PHYSICAL, ARCHER_DAMAGE);
            Play(VFX_ARROW_HIT, WorldToScreen(a->target->position));
            Vec2 hand = TRS(XY(a->position), 0.0f, a->scale) * a->animator.bones[BONE_STICK_HAND_B] * VEC2_ZERO;
            CreateArrow(
                a->team,
                Vec3{hand.x, hand.y, 0.0f},
                XY(a->target->position),
                4.0f);
        }
    }
#endif

    // Apply physics update (velocity, friction, position, state management)
    // This also handles animation selection based on state
    UpdateUnit(a);

    // Update animator
}

ArcherEntity* CreateArcher(Team team, const Vec3& position) {
    static EntityVtable vtable = {
        .update = UpdateArcher,
        .draw = DrawArcher,
        .draw_shadow = DrawArcherShadow,
        .death = KillArcher
    };

    ArcherEntity* a = static_cast<ArcherEntity*>(CreateUnit(
        UNIT_TYPE_ARCHER,
        team,
        vtable,
        position,
        0.0f,
        {GetTeamDirection(team).x, 1.0f}));
    a->health = ARCHER_HEALTH;
    a->size = ARCHER_SIZE;
    a->cooldown = RandomFloat(ARCHER_COOLDOWN_MIN, ARCHER_COOLDOWN_MAX);

    Init(a->animator, SKELETON_STICK);
    Play(a->animator, ANIMATION_ARCHER_IDLE, 1.0f, true);
    return a;
}

static void FireArrow(UnitEntity* u, UnitEntity* target) {
    ArcherEntity* a = static_cast<ArcherEntity*>(u);
    Vec2 hand = TransformPoint(TRS(VEC2_ZERO, 0.0f, a->scale) * a->animator.bones[BONE_STICK_HAND_B]);
    CreateArrow(
        a->team,
        a->position + Vec3{hand.x, hand.y, 0.0f},
        target->position + Vec3{0.0f, target->info->height * 0.5f, 0.0f},
        4.0f);
}

void InitArcherUnit() {
    InitUnitInfo({
        .type = UNIT_TYPE_ARCHER,
        .name = GetName("Archer"),
        .size = ARCHER_SIZE,
        .height = ARCHER_HEIGHT,
        .range = ARCHER_RANGE,
        .speed = ARCHER_SPEED,
        .create_func = (UnitCreateFunc)CreateArcher,
        .attack_func = (UnitAttackFunc)FireArrow,
        .icon_mesh = MESH_COWBOY_ICON,
        .idle_animation = ANIMATION_ARCHER_IDLE,
        .move_animation = ANIMATION_STICK_RUN,
        .shuffle_animation = ANIMATION_ARCHER_SHUFFLE,
        .attack_animation = ANIMATION_ARCHER_ATTACK,
        .reload_animation = ANIMATION_ARCHER_RELOAD,
    });
}

ArcherEntity* CreateArcher2(Team team, const Vec3& position) {
    static EntityVtable vtable = {
        .update = UpdateArcher,
        .draw = DrawArcher,
        .draw_shadow = DrawArcherShadow,
        .death = KillArcher
    };

    ArcherEntity* a = static_cast<ArcherEntity*>(CreateUnit(UNIT_TYPE_COWBOY, team, vtable, position, 0.0f, {GetTeamDirection(team).x, 1.0f}));
    a->health = ARCHER_HEALTH;
    a->size = ARCHER_SIZE;
    a->cooldown = RandomFloat(ARCHER_COOLDOWN_MIN, ARCHER_COOLDOWN_MAX);

    Init(a->animator, SKELETON_STICK);
    Play(a->animator, ANIMATION_ARCHER_IDLE, 1.0f, true);
    return a;
}

void InitCowboyUnit() {
    InitUnitInfo({
        .type = UNIT_TYPE_COWBOY,
        .name = GetName("Cowboy"),
        .size = ARCHER_SIZE,
        .range = 1.0f,
        .speed = ARCHER_SPEED,
        .create_func = (UnitCreateFunc)CreateArcher2,
        .icon_mesh = MESH_COWBOY_ICON,
        .idle_animation = ANIMATION_ARCHER_IDLE,
        .move_animation = ANIMATION_STICK_RUN,
        .shuffle_animation = ANIMATION_ARCHER_SHUFFLE,
        .attack_animation = nullptr
    });
}