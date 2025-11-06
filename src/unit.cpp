//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#include "rvo.h"

constexpr float UNIT_MIN_SPEED = 1.0f;
constexpr float UNIT_SHUFFLE_SPEED = 0.1f;
constexpr float UNIT_SHUFFLE_SPEED_SQR = UNIT_SHUFFLE_SPEED * UNIT_SHUFFLE_SPEED;

struct CollectRVOAgentsArgs {
    UnitEntity* unit;
    RVOAgent agents[MAX_UNITS];
    int count;
};

struct FindClosestArgs {
    UnitEntity* unit;
    Vec3 position;
    UnitEntity* target;
    float target_distance_sqr;
};

struct FindClosestEnemyArgs {
    UnitEntity* unit;
    UnitEntity* closest;
    float closest_distance_sqr;
};

struct UnitCallbackArgs {
    Team team;
    void* user_data;
    bool (*callback)(UnitEntity* unit, void* user_data);
};

static bool UnitCallback(u32 item_index, void* item_ptr, void* user_data) {
    (void)item_index;

    assert(item_ptr);
    Entity* e = static_cast<Entity*>(item_ptr);
    if (e->type != ENTITY_TYPE_UNIT)
        return true;

    UnitCallbackArgs* args = static_cast<UnitCallbackArgs*>(user_data);
    UnitEntity* u = static_cast<UnitEntity*>(e);
    if (args->team != TEAM_UNKNOWN && u->team != args->team)
        return true;

    return args->callback(u, args->user_data);
}

void EnumerateUnits(Team team, bool (*callback)(UnitEntity* unit, void* user_data), void* user_data) {
    UnitCallbackArgs args = { team, user_data, callback };
    Enumerate(g_game.entity_allocator, UnitCallback, &args);
}

void Damage(UnitEntity* u, DamageType damage_type, float amount) {
    (void) damage_type;
    u->health -= amount;

    if (u->health < 0.0f) {
        if (u->vtable.death) {
            u->vtable.death(u, damage_type);
        } else {
            HandleUnitDeath(u, damage_type);
            Free(u);
        }
    }
}

static bool EnumerateClosestUnit(UnitEntity* u, void* user_data) {
    assert(u);
    assert(user_data);

    if (u->health <= 0.0f)
        return true;

    FindClosestArgs* args = static_cast<FindClosestArgs*>(user_data);
    float distance_sqr = DistanceSqr(u, args->position);
    if (distance_sqr >= args->target_distance_sqr)
        return true;

    args->target = u;
    args->target_distance_sqr = distance_sqr;
    return true;
}

UnitEntity* FindClosestEnemy(UnitEntity* unit) {
    FindClosestArgs args {
        .unit = unit,
        .position = unit->position,
        .target = nullptr,
        .target_distance_sqr = F32_MAX
    };
    EnumerateUnits(GetOppositeTeam(unit->team), EnumerateClosestUnit, &args);
    return args.target;
}

UnitEntity* FindClosestUnit(const Vec3& position) {
    FindClosestArgs args {
        .unit = nullptr,
        .position = position,
        .target = nullptr,
        .target_distance_sqr = F32_MAX
    };
    EnumerateUnits(TEAM_UNKNOWN, EnumerateClosestUnit, &args);
    return args.target;
}

UnitEntity* CreateUnit(UnitType type, Team team, const EntityVtable& vtable, const Vec3& position, float rotation, const Vec2& scale) {
    UnitEntity* u = static_cast<UnitEntity*>(CreateEntity(ENTITY_TYPE_UNIT, vtable, position, rotation, scale));
    u->state = UNIT_STATE_IDLE;
    u->team = team;
    u->unit_type = type;
    u->velocity = VEC3_ZERO;
    u->desired_velocity = VEC3_ZERO;
    u->target = {};
    u->info = GetUnitInfo(type);
    return u;
}

static bool CollectRVOAgent(UnitEntity* u, void* user_data) {
    assert(u);
    assert(user_data);
    CollectRVOAgentsArgs* args = static_cast<CollectRVOAgentsArgs*>(user_data);

    if (u == args->unit || u->health <= 0.0f)
        return true;

    // Only consider nearby units (within 5 units)
    float distance_sq = DistanceSqr(args->unit, u->position);
    if (distance_sq > 25.0f)
        return true;

    if (args->count >= 64)
        return true;

    // Add this unit as an obstacle
    args->agents[args->count].position = u->position;
    args->agents[args->count].velocity = u->velocity;
    args->agents[args->count].radius = u->size;
    args->agents[args->count].max_speed = 1.0f;
    args->count++;

    return true;
}

Vec3 ComputeRVOVelocityForUnit(UnitEntity* u, const Vec3& preferred_velocity, float max_speed) {
    CollectRVOAgentsArgs args = {
        .unit = u,
        .agents = {},
        .count = 0
    };
    EnumerateUnits(u->team, CollectRVOAgent, &args);

    RVOAgent agent = {
        .position = u->position,
        .velocity = u->velocity,
        .preferred_velocity = preferred_velocity,
        .radius = u->size,
        .max_speed = max_speed
    };

    return ComputeRVOVelocity(agent, args.agents, args.count, 1.5f);
}

void ApplyImpulse(UnitEntity* u, const Vec3& impulse) {
    u->velocity += impulse;
}

static void UpdateVelocity(UnitEntity* u) {
    UnitEntity* target = GetUnit(u->target);
    if (target && DistanceSqr(target, u) > Sqr(u->info->range)) {
        Vec3 desired_velocity = Direction(u, target) * u->info->speed;
        u->desired_velocity = ComputeRVOVelocityForUnit(u, desired_velocity, u->info->speed);
    } else {
        u->desired_velocity = ComputeRVOVelocityForUnit(u, VEC3_ZERO, u->info->speed);
    }

    float dt = GetGameFrameTime();
    u->velocity = u->desired_velocity;
    float speed = Clamp(Length(u->velocity), 0.0f, u->info->speed);
    speed = Max(UNIT_MIN_SPEED, speed);
    u->velocity = Normalize(u->velocity) * speed;
    u->position += u->velocity * dt;
}

static void UpdateMoveState(UnitEntity* u) {
    UpdateVelocity(u);

    // Determine which animation to play based on desired velocity speed
    float speed_sqr = LengthSqr(u->velocity);

    Animation* target_animation = nullptr;
    if (speed_sqr > UNIT_SHUFFLE_SPEED_SQR) {
        target_animation = u->info->move_animation;
    } else if (speed_sqr >= UNIT_MIN_SPEED * UNIT_MIN_SPEED) {
        target_animation = u->info->shuffle_animation;
    } else {
        SetState(u, UNIT_STATE_IDLE);
        return;
    }

    if (target_animation && u->animator.animation != target_animation)
        Play(u->animator, target_animation, 1.0f, true);
}

static void UpdateIdleState(UnitEntity* u) {
    UpdateVelocity(u);

    if (LengthSqr(u->velocity) > 0) {
        SetState(u, UNIT_STATE_MOVE);
        return;
    }

    if (u->target) {
        UnitEntity* target = GetUnit(u->target);
        float target_dist_sqr = DistanceSqr(u, target);
        float desired_target_dist_sqr = u->info->range * u->info->range;
        if (target_dist_sqr > desired_target_dist_sqr) {
            SetState(u, UNIT_STATE_MOVE);
            return;
        } else {
            SetState(u, UNIT_STATE_RELOAD);
            return;
        }
    }

    // todo: attack
}

static void UpdateReloadState(UnitEntity* u) {
    if (IsPlaying(u->animator))
        return;

    if (u->target) {
        SetState(u, UNIT_STATE_ATTACK);
        return;
    }

    SetState(u, UNIT_STATE_IDLE);
}

static void UpdateAttackState(UnitEntity* u) {
    if (IsPlaying(u->animator))
        return;

    if (u->target) {
        SetState(u, UNIT_STATE_RELOAD);
    } else {
        SetState(u, UNIT_STATE_IDLE);
    }
}

static void SetIdleState(UnitEntity* u) {
    if (u->animator.animation != u->info->idle_animation)
        Play(u->animator, u->info->idle_animation, 1.0f, true);
}

static void SetMoveState(UnitEntity* e) {
    UpdateMoveState(e);
}

static void SetReloadState(UnitEntity* u) {
    Play(u->animator, u->info->reload_animation, 1.0f, false);
}

static void SetAttackState(UnitEntity* u) {
    if (u->info->attack_func && u->target) {
        UnitEntity* target = GetUnit(u->target);
        assert(target);
        u->info->attack_func(u, target);
    }
    Play(u->animator, u->info->attack_animation, 1.0f, false);
}

void SetState(UnitEntity* u, UnitState new_state) {
    u->state = new_state;
    u->state_time = 0.0f;

    if (new_state == UNIT_STATE_IDLE)
        SetIdleState(u);
    else if (new_state == UNIT_STATE_MOVE)
        SetMoveState(u);
    else if (new_state == UNIT_STATE_RELOAD)
        SetReloadState(u);
    else if (new_state == UNIT_STATE_ATTACK)
        SetAttackState(u);
}

static void UpdateTarget(UnitEntity* u) {
    UnitEntity* target = GetUnit(u->target);
    if (target && target->health > 0.0f)
        return;

    target = FindClosestEnemy(u);
    if (!target)
        return;

    u->target = GetHandle(target);
}

void UpdateUnit(UnitEntity* u) {
    UpdateTarget(u);

    if (u->state == UNIT_STATE_IDLE)
        UpdateIdleState(u);
    else if (u->state == UNIT_STATE_MOVE)
        UpdateMoveState(u);
    else if (u->state == UNIT_STATE_ATTACK)
        UpdateAttackState(u);
    else if (u->state == UNIT_STATE_RELOAD)
        UpdateReloadState(u);
}

void DrawGizmos(UnitEntity* u, const Mat3& transform) {
    BindColor(COLOR_WHITE);
    BindDepth(GetApplicationTraits()->renderer.max_depth * 0.9f);
    BindMaterial(g_game.material);
    Mesh* state_mesh = nullptr;
    if (u->state == UNIT_STATE_ATTACK) {
        state_mesh = MESH_ICON_STATE_ATTACK;
    } else if (u->state == UNIT_STATE_RELOAD) {
        state_mesh = MESH_ICON_STATE_RELOAD;
    } else if (u->state == UNIT_STATE_MOVE) {
        state_mesh = MESH_ICON_STATE_MOVE;
    }

    if (state_mesh)
        DrawMesh(state_mesh, transform * Translate(Vec2{0,1.1f}) * Scale(0.35f));
}