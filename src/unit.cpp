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
        HandleUnitDeath(u, damage_type);
        Free(u);
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
    float distance = Distance(args->position, u->position);
    if (distance < args->target_distance) {
        args->target = u;
        args->target_distance = distance;
    }
    return true;
}

UnitEntity* FindClosestEnemy(UnitEntity* unit) {
    FindTargetArgs args {
        .unit = unit,
        .position = unit->position,
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

UnitEntity* CreateUnit(UnitType type, Team team, const EntityVtable& vtable, const Vec2& position, float rotation, const Vec2& scale) {
    UnitEntity* u = static_cast<UnitEntity*>(CreateEntity(ENTITY_TYPE_UNIT, vtable, position, rotation, scale));
    u->state = UNIT_STATE_IDLE;
    u->team = team;
    u->unit_type = type;
    return u;
}
