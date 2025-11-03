//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

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
    return u;
}

struct AvoidVelocityArgs {
    UnitEntity* unit;
    Vec2 velocity;
    int count;
};

static bool EnumerateAvoidVelocity(UnitEntity* u, void* user_data) {
    assert(u);
    assert(user_data);
    AvoidVelocityArgs* args = static_cast<AvoidVelocityArgs*>(user_data);
    if (u == args->unit || u->health <= 0.0f)
        return true;

    float avoidance_radius = (u->size + args->unit->size);
    float distance = Distance(XY(args->unit->position), XY(u->position));

    if (distance > avoidance_radius)
        return true;

    // Avoid division by zero if units are exactly on top of each other
    if (distance < 0.01f)
        distance = 0.01f;

    Vec2 direction = Normalize(XY(args->unit->position) - XY(u->position));

    // Use quadratic falloff for more aggressive avoidance when close
    float normalized_distance = distance / avoidance_radius;
    float strength = (1.0f - normalized_distance) * (1.0f - normalized_distance);

    // Accumulate forces (don't average yet)
    args->velocity += direction * strength;
    args->count++;

    return true;
}


float GetAvoidVelocity(UnitEntity* u, Vec2* out_velocity) {
    AvoidVelocityArgs args = {
        .unit = u,
        .velocity = VEC2_ZERO,
        .count = 0
    };;
    EnumerateUnits(u->team, EnumerateAvoidVelocity, &args);
    if (args.count == 0) {
        *out_velocity = VEC2_ZERO;
        return 0.0f;
    }

    // Return the magnitude (strength) of avoidance needed
    float strength = Length(args.velocity);

    // Normalize for direction
    *out_velocity = Normalize(args.velocity);

    return strength;
}
