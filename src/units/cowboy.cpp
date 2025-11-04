//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

constexpr float COWBOY_SPEED = 2.0f;
constexpr float COWBOY_RANGE = 6.0f;
constexpr float COWBOY_COOLDOWN_MIN = 1.4f;
constexpr float COWBOY_COOLDOWN_MAX = 1.6f;
constexpr float COWBOY_DAMAGE = 0.75f;
constexpr float COWBOY_HEALTH = 5.0f;
constexpr float COWBOY_SIZE = 1;
constexpr float COWBOY_BULLET_SPEED = 20.0f;

inline CowboyEntity* CastCowboy(Entity* e) {
    assert(e && e->type == ENTITY_TYPE_UNIT);
    CowboyEntity* a = static_cast<CowboyEntity*>(e);
    assert(a->unit_type == UNIT_TYPE_COWBOY);
    return a;
}

static void DrawCowboyInternal(Entity*, const Mat3&, bool) {
#if 0
    CowboyEntity* a = CastCowboy(e);
    DrawMesh(MESH_HUMAN_FOOT_L, transform, e->animator, BONE_COWBOY_FOOT_L);
    DrawMesh(MESH_HUMAN_FOOT_R, transform, e->animator, BONE_COWBOY_FOOT_R);
    DrawMesh(MESH_HUMAN_LEG_L, transform, e->animator, BONE_COWBOY_LEFT_L);
    DrawMesh(MESH_HUMAN_LEG_R, transform, e->animator, BONE_COWBOY_LEFT_R);
    DrawMesh(MESH_HUMAN_HAND_R, transform, e->animator, BONE_COWBOY_HAND_R);
    DrawMesh(MESH_COWBOY_PISTOL, transform, e->animator, BONE_COWBOY_HAND_R);
    DrawMesh(MESH_COWBOY_ARM_R, transform, e->animator, BONE_COWBOY_ARM_R);
    DrawMesh(MESH_COWBOY_BODY, transform, e->animator, BONE_COWBOY_CHEST);
    if (!shadow) DrawMesh(MESH_COWBOY_TIE, transform, e->animator, BONE_COWBOY_TIE);
    DrawMesh(MESH_COWBOY_ARM_L, transform, e->animator, BONE_COWBOY_ARM_L);
    DrawMesh(MESH_HUMAN_HAND_L, transform, e->animator, BONE_COWBOY_HAND_L);
    DrawMesh(MESH_COWBOY_HEAD, transform, e->animator, BONE_COWBOY_HEAD);
    if (!shadow) {
        DrawMesh(a->health <= 0 ? MESH_HUMAN_EYE_DEAD : MESH_HUMAN_EYE, transform, e->animator, BONE_COWBOY_EYE_L);
        DrawMesh(a->health <= 0 ? MESH_HUMAN_EYE_DEAD : MESH_HUMAN_EYE, transform, e->animator, BONE_COWBOY_EYE_R);
    }
    DrawMesh(MESH_COWBOY_MUSTACHE, transform, e->animator, BONE_COWBOY_MUSTACHE);
#endif
}

void DrawCowboy(Entity* e, const Mat3& transform) {
    CowboyEntity* a = CastCowboy(e);
    BindColor(COLOR_WHITE, GetTeamColorOffset(a->team));
    BindMaterial(g_game.material);
    BindDepth(2.0f - (a->position.y / 10.0f));
    DrawCowboyInternal(a, transform, false);
    BindDepth(0.0f);
}

void DrawCowboyShadow(Entity* e, const Mat3& transform) {
    CowboyEntity* a = CastCowboy(e);
    DrawCowboyInternal(a, transform, true);
}

struct FindCowboyTargetArgs {
    CowboyEntity* a;
    UnitEntity* target;
    float target_distance;
};

bool FindCowboyTarget(UnitEntity* u, void* user_data) {
    assert(u);
    assert(user_data);
    if (u->health <= 0.0f)
        return true;

    FindCowboyTargetArgs* args = static_cast<FindCowboyTargetArgs*>(user_data);
    float distance = Distance(XY(args->a->position), XY(u->position));
    if (distance < args->target_distance) {
        args->target = u;
        args->target_distance = distance;
    }
    return true;
}

void UpdateCowboyDead(Entity* e) {
    CowboyEntity* a = CastCowboy(e);
    Update(a->animator, GetGameTimeScale());
}

void UpdateCowboy(Entity*) {
#if 0
    CowboyEntity* a = CastCowboy(e);
    FindCowboyTargetArgs args {
        .a = a,
        .target = nullptr,
        .target_distance = F32_MAX
    };
    EnumerateUnits(GetOppositeTeam(a->team), FindCowboyTarget, &args);
    if (!args.target)
        return;

    if (args.target_distance > COWBOY_RANGE) {
        if (a->animator.animation != ANIMATION_COWBOY_RUN)
            Play(a->animator, ANIMATION_COWBOY_RUN, 1.0f, true);
        MoveTowards(a, XY(args.target->position), COWBOY_SPEED);
        a->cooldown = RandomFloat(COWBOY_COOLDOWN_MIN, COWBOY_COOLDOWN_MAX);
    } else {
        a->cooldown -= GetGameFrameTime();
        if (a->cooldown <= 0.0f) {
            a->cooldown = RandomFloat(COWBOY_COOLDOWN_MIN, COWBOY_COOLDOWN_MAX);
            Damage(args.target, DAMAGE_TYPE_PHYSICAL, COWBOY_DAMAGE);
            Vec2 gun = TransformPoint(TRS(XY(a->position), 0.0f, a->scale) * a->animator.bones[BONE_COWBOY_HAND_R]);
            Play(VFX_BOW_FIRE, WorldToScreen(Vec3{gun.x, gun.y, 0.0f}));
            Play(a->animator, ANIMATION_COWBOY_SHOOT, 1.0f, false);
            Play(SOUND_REVOLVER_FIRE_A, 0.5f, RandomFloat(0.95f, 1.05f));
            CreateBullet(a->team, Vec3{gun.x, gun.y, 0.0f}, XY(args.target->position), COWBOY_BULLET_SPEED);
        } else if (a->animator.animation != ANIMATION_COWBOY_IDLE && a->animator.animation != ANIMATION_COWBOY_SHOOT) {
            Play(a->animator, ANIMATION_COWBOY_IDLE, 1.0f, true);
        }
    }

    Update(a->animator);
#endif
}

void KillCowboy(Entity* e, DamageType damage_type) {
    UnitEntity* u = static_cast<UnitEntity*>(e);
    u->vtable.update = UpdateCowboyDead;
    HandleUnitDeath(u, damage_type);
    //Play(u->animator, ANIMATION_COWBOY_DEATH, 0.5f, false);
    // Free(e);
}


