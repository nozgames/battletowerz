//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

constexpr float COWBOY_SPEED = 5.0f;
constexpr float COWBOY_RANGE = 8.0f;
constexpr float COWBOY_COOLDOWN = 1.5f;
constexpr float COWBOY_DAMAGE = 0.75f;
constexpr float COWBOY_HEALTH = 5.0f;
constexpr float COWBOY_SIZE = 0.25f;

inline CowboyEntity* CastCowboy(Entity* e) {
    assert(e && e->type == ENTITY_TYPE_UNIT);
    CowboyEntity* a = static_cast<CowboyEntity*>(e);
    assert(a->unit_type == UNIT_TYPE_ARCHER);
    return a;
}

void DrawCowboy(Entity* e, const Mat3& transform_a) {
    CowboyEntity* a = CastCowboy(e);
    BindColor(COLOR_WHITE, GetTeamColorOffset(a->team));
    BindMaterial(g_game.material);

    Mat3 transform = transform_a * Scale(1);

    BindDepth(2.0f - (a->position.y / 10.0f));
    DrawMesh(MESH_HUMAN_FOOT_L, transform, e->animator, BONE_COWBOY_FOOT_L);
    DrawMesh(MESH_HUMAN_FOOT_R, transform, e->animator, BONE_COWBOY_FOOT_R);
    DrawMesh(MESH_HUMAN_LEG_L, transform, e->animator, BONE_COWBOY_LEFT_L);
    DrawMesh(MESH_HUMAN_LEG_R, transform, e->animator, BONE_COWBOY_LEFT_R);
    DrawMesh(MESH_HUMAN_HAND_R, transform, e->animator, BONE_COWBOY_HAND_R);
    DrawMesh(MESH_COWBOY_PISTOL, transform, e->animator, BONE_COWBOY_HAND_R);
    DrawMesh(MESH_COWBOY_ARM_R, transform, e->animator, BONE_COWBOY_ARM_R);
    DrawMesh(MESH_COWBOY_BODY, transform, e->animator, BONE_COWBOY_CHEST);
    DrawMesh(MESH_COWBOY_TIE, transform, e->animator, BONE_COWBOY_TIE);
    DrawMesh(MESH_COWBOY_ARM_L, transform, e->animator, BONE_COWBOY_ARM_L);
    DrawMesh(MESH_HUMAN_HAND_L, transform, e->animator, BONE_COWBOY_HAND_L);
    DrawMesh(MESH_COWBOY_HEAD, transform, e->animator, BONE_COWBOY_HEAD);
    DrawMesh(MESH_HUMAN_EYE, transform, e->animator, BONE_COWBOY_EYE_L);
    DrawMesh(MESH_HUMAN_EYE, transform, e->animator, BONE_COWBOY_EYE_R);
    DrawMesh(MESH_COWBOY_MUSTACHE, transform, e->animator, BONE_COWBOY_MUSTACHE);
    BindDepth(0.0f);
}


struct FindCowboyTargetArgs {
    CowboyEntity* a;
    UnitEntity* target;
    float target_distance;
};

static bool FindCowboyTarget(UnitEntity* u, void* user_data)
{
    assert(u);
    assert(user_data);
    FindCowboyTargetArgs* args = static_cast<FindCowboyTargetArgs*>(user_data);
    float distance = Distance(XY(args->a->position), XY(u->position));
    if (distance < args->target_distance) {
        args->target = u;
        args->target_distance = distance;
    }
    return true;
}

void UpdateCowboy(Entity* e) {
    CowboyEntity* a = CastCowboy(e);
    FindCowboyTargetArgs args {
        .a = a,
        .target = nullptr,
        .target_distance = F32_MAX
    };
    EnumerateUnits(GetOppositeTeam(a->team), FindCowboyTarget, &args);
    if (!args.target)
        return;

    if (!IsPlaying(a->animator) && !IsLooping(a->animator)) {
        // Vec2 hand = TRS(XY(a->position), 0.0f, a->scale) * a->animator.bones[BONE_COWBOY_HAND_R] * VEC2_ZERO;
        Play(a->animator, ANIMATION_COWBOY_IDLE, 1.0f, true);
        // CreateArrow(
        //     a->team,
        //     Vec3{hand.x, hand.y, 0.0f},
        //     XY(args.target->position),
        //     4.0f);
    }

    if (args.target_distance > COWBOY_RANGE) {
        MoveTowards(a, XY(args.target->position), COWBOY_SPEED);
        a->cooldown = COWBOY_COOLDOWN;
    } else {
        a->cooldown -= GetGameFrameTime();
        if (a->cooldown <= 0.0f) {
            a->cooldown = COWBOY_COOLDOWN;
            //     Play(a->animator, ANIMATION_COWBOY_DRAW, 1.0f, false);
            //Damage(args.target, DAMAGE_TYPE_PHYSICAL, COWBOY_DAMAGE);
            Vec2 gun = TransformPoint(TRS(XY(a->position), 0.0f, a->scale) * a->animator.bones[BONE_COWBOY_HAND_R]);
            Play(VFX_BOW_FIRE, WorldToScreen(Vec3{gun.x, gun.y, 0.0f}));
            Play(a->animator, ANIMATION_COWBOY_SHOOT, 1.0f, false);
        }
    }

    Update(a->animator);
}

CowboyEntity* CreateCowboy(Team team, const Vec3& position)
{
    static EntityVtable vtable = {
        .update = UpdateCowboy,
        .render = DrawCowboy
    };

    CowboyEntity* a = static_cast<CowboyEntity*>(CreateUnit(UNIT_TYPE_ARCHER, team, vtable, position, 0.0f, {GetTeamDirection(team).x, 1.0f}));
    a->health = COWBOY_HEALTH;
    a->size = COWBOY_SIZE;

    Init(a->animator, SKELETON_COWBOY);
    Play(a->animator, ANIMATION_COWBOY_IDLE, 1.0f, true);
    return a;
}