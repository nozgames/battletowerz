//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#include "rvo.h"

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

struct FindTargetArgs {
    UnitEntity* unit;
    Vec2 position;
    UnitEntity* target;
    float target_distance;
};

static bool EnumerateClosestEnemy(UnitEntity* u, void* user_data) {
    assert(u);
    assert(user_data);
    FindTargetArgs* args = static_cast<FindTargetArgs*>(user_data);
    float distance = Distance(args->position, XY(u->position));
    if (distance < args->target_distance) {
        args->target = u;
        args->target_distance = distance;
    }
    return true;
}

UnitEntity* FindClosestEnemy(UnitEntity* unit) {
    FindTargetArgs args {
        .unit = unit,
        .position = XY(unit->position),
        .target = nullptr,
        .target_distance = F32_MAX
    };
    EnumerateUnits(GetOppositeTeam(unit->team), EnumerateClosestEnemy, &args);
    return args.target;
}

UnitEntity* FindClosestUnit(const Vec2& position) {
    FindTargetArgs args {
        .unit = nullptr,
        .position = position,
        .target = nullptr,
        .target_distance = F32_MAX
    };
    EnumerateUnits(TEAM_UNKNOWN, EnumerateClosestEnemy, &args);
    return args.target;
}

void MoveTowards(UnitEntity* unit, const Vec2& target_position, float speed, const Vec2& avoid_velocity, float avoid_weight) {
    // Combine target direction with weighted avoidance velocity, then normalize
    Vec2 target_direction = Normalize(target_position - XY(unit->position));
    Vec2 combined_direction = target_direction + (avoid_velocity * avoid_weight);

    // Normalize to maintain consistent speed
    Vec2 normalized_direction = Normalize(combined_direction);

    Vec2 move = normalized_direction * speed * GetGameFrameTime();
    unit->position.x += move.x;
    unit->position.y += move.y;
}

UnitEntity* CreateUnit(UnitType type, Team team, const EntityVtable& vtable, const Vec3& position, float rotation, const Vec2& scale) {
    UnitEntity* u = static_cast<UnitEntity*>(CreateEntity(ENTITY_TYPE_UNIT, vtable, position, rotation, scale));
    u->state = UNIT_STATE_IDLE;
    u->team = team;
    u->unit_type = type;
    u->velocity = VEC2_ZERO;
    u->desired_velocity = VEC2_ZERO;
    u->target = {};
    u->info = GetUnitInfo(type);
    return u;
}

struct CollectRVOAgentsArgs {
    UnitEntity* unit;
    RVOAgent agents[MAX_UNITS];
    int count;
};

static bool CollectRVOAgent(UnitEntity* u, void* user_data) {
    assert(u);
    assert(user_data);
    CollectRVOAgentsArgs* args = static_cast<CollectRVOAgentsArgs*>(user_data);

    if (u == args->unit || u->health <= 0.0f)
        return true;

    // Only consider nearby units (within 5 units)
    float distance_sq = DistanceSqr(XY(args->unit->position), XY(u->position));
    if (distance_sq > 25.0f)
        return true;

    if (args->count >= 64)
        return true;

    // Add this unit as an obstacle
    args->agents[args->count].position = XY(u->position);
    args->agents[args->count].velocity = u->velocity;
    args->agents[args->count].radius = u->size;
    args->agents[args->count].max_speed = 1.0f;
    args->count++;

    return true;
}

Vec2 ComputeRVOVelocityForUnit(UnitEntity* u, const Vec2& preferred_velocity, float max_speed) {
    CollectRVOAgentsArgs args = {
        .unit = u,
        .agents = {},
        .count = 0
    };
    EnumerateUnits(u->team, CollectRVOAgent, &args);

    RVOAgent agent = {
        .position = XY(u->position),
        .velocity = u->velocity,
        .preferred_velocity = preferred_velocity,
        .radius = u->size,
        .max_speed = max_speed
    };

    return ComputeRVOVelocity(agent, args.agents, args.count, 1.5f);
}

void ApplyImpulse(UnitEntity* u, const Vec2& impulse) {
    // Directly add to velocity (for knockback, explosions, etc.)
    u->velocity.x += impulse.x;
    u->velocity.y += impulse.y;
}

static void UpdateVelocity(UnitEntity* u) {
    float dt = GetGameFrameTime();
    constexpr float FRICTION = 8.0f;

    Vec2 velocity_error = u->desired_velocity - u->velocity;
    u->velocity += velocity_error * u->acceleration * dt;
    u->velocity *= expf(-FRICTION * dt);
    u->position += ToVec3(u->velocity * dt);
}

static void UpdateMoveState(UnitEntity* u) {
    UnitEntity* target = GetUnit(u->target);
    if (target && DistanceSqr(XY(target->position), XY(u->position)) > u->info->range * u->info->range) {
        Vec2 desired_velocity = Normalize(XY(target->position) - XY(u->position)) * u->info->speed;
        u->desired_velocity = ComputeRVOVelocityForUnit(u, desired_velocity, u->info->speed);
    } else {
        u->desired_velocity = VEC2_ZERO;
    }

    UpdateVelocity(u);

    // Determine which animation to play based on desired velocity speed
    float speed_sqr = LengthSqr(u->desired_velocity);
    constexpr float SHUFFLE_THRESHOLD = 0.5f;
    constexpr float SHUFFLE_THRESHOLD_SQR = SHUFFLE_THRESHOLD * SHUFFLE_THRESHOLD;

    Animation* target_animation = nullptr;
    if (speed_sqr > SHUFFLE_THRESHOLD_SQR) {
        target_animation = u->info->move_animation;
    } else if (speed_sqr > F32_EPSILON) {
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
        Entity* target = GetEntity(u->target);
        float target_dist_sqr = DistanceSqr(XY(u->position), XY(target->position));
        float desired_target_dist_sqr = u->info->range * u->info->range;
        if (target_dist_sqr > desired_target_dist_sqr) {
            SetState(u, UNIT_STATE_MOVE);
            return;
        }
    }

    // todo: attack
}

static void UpdateAttackingState(UnitEntity* u) {
    const UnitInfo* info = GetUnitInfo(u->unit_type);

    // Apply subtle RVO adjustments to maintain spacing
    Vec2 preferred_velocity = VEC2_ZERO;  // Prefer to stay in place
    u->desired_velocity = ComputeRVOVelocityForUnit(u, preferred_velocity, u->acceleration * 0.3f);

    // Apply physics
    UpdateVelocity(u);

    // Check if target moved out of range
    if (u->target) {
        Entity* target = GetEntity(u->target);
        float distance = Distance(XY(u->position), XY(target->position));
        if (distance > info->range) {
            SetState(u, UNIT_STATE_MOVE);
            return;
        }
    } else {
        // Lost target
        SetState(u, UNIT_STATE_IDLE);
        return;
    }

    // Play idle/attack animation (unit-specific code will handle actual attacking)
    if (info->idle_animation && u->animator.animation != info->idle_animation) {
        Play(u->animator, info->idle_animation, 1.0f, true);
    }
}

static void SetIdleState(UnitEntity* u) {
    if (u->animator.animation != u->info->idle_animation)
        Play(u->animator, u->info->idle_animation, 1.0f, true);
}

static void SetMoveState(UnitEntity* e) {
    UpdateMoveState(e);
}

void SetState(UnitEntity* u, UnitState new_state) {
    u->state = new_state;

    if (new_state == UNIT_STATE_IDLE)
        SetIdleState(u);
    else if (new_state == UNIT_STATE_MOVE)
        SetMoveState(u);
}

struct FindClosestEnemyArgs {
    UnitEntity* unit;
    UnitEntity* closest;
    float closest_distance_sq;
};

static bool EnumerateFindClosestEnemy(UnitEntity* u, void* user_data) {
    assert(u);
    assert(user_data);
    FindClosestEnemyArgs* args = static_cast<FindClosestEnemyArgs*>(user_data);

    if (u->health <= 0.0f)
        return true;

    float distance_sq = DistanceSqr(XY(args->unit->position), XY(u->position));
    if (distance_sq < args->closest_distance_sq) {
        args->closest = u;
        args->closest_distance_sq = distance_sq;
    }

    return true;
}

static void UpdateTarget(UnitEntity* u) {
    UnitEntity* target = GetUnit(u->target);
    if (target && target->health > 0.0f)
        return;

    FindClosestEnemyArgs args = {
        .unit = u,
        .closest = nullptr,
        .closest_distance_sq = F32_MAX
    };

    EnumerateUnits(GetOppositeTeam(u->team), EnumerateFindClosestEnemy, &args);
    u->target = GetHandle(args.closest);
}

void UpdateUnit(UnitEntity* u) {
    UpdateTarget(u);

    if (u->state == UNIT_STATE_IDLE)
        UpdateIdleState(u);
    else if (u->state == UNIT_STATE_MOVE)
        UpdateMoveState(u);
    else if (u->state == UNIT_STATE_ATTACKING)
        UpdateAttackingState(u);
}
