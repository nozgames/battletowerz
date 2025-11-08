//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#pragma once

struct UnitInfo;

enum Team {
    TEAM_UNKNOWN=-1,
    TEAM_RED,
    TEAM_BLUE,
    TEAM_COUNT
};

enum UnitType {
    UNIT_TYPE_UNKNOWN = -1,
    UNIT_TYPE_ARCHER,
    UNIT_TYPE_COWBOY,
    UNIT_TYPE_KNIGHT,
    UNIT_TYPE_TOWER,
    UNIT_TYPE_COUNT
};

enum UnitState {
    UNIT_STATE_IDLE,
    UNIT_STATE_MOVE,
    UNIT_STATE_RELOAD,
    UNIT_STATE_ATTACK,
    UNIT_STATE_DEAD,
    UNIT_STATE_COUNT
};

struct UnitEntity : Entity {
    UnitType unit_type;
    UnitState state;
    Team team;
    float health;
    float size;
    float state_time;
    Vec3 velocity;
    Vec3 desired_velocity;
    EntityHandle target;
    const UnitInfo* info;
    float target_switch_cooldown;
};

struct ArcherEntity : UnitEntity {
    float cooldown;
};

struct KnightEntity : UnitEntity {
    float cooldown;
};

struct CowboyEntity : UnitEntity {
    float cooldown;
};

struct TowerEntity : UnitEntity {
};

union FatUnitEntity {
    UnitEntity unit;
    ArcherEntity archer;
    KnightEntity knight;
    TowerEntity tower;
};

typedef UnitEntity* (*UnitCreateFunc)(Team team, const Vec3& position);
typedef void (*UnitAttackFunc)(UnitEntity* u, UnitEntity* target);

struct UnitInfo {
    UnitType type;
    const Name* name;
    float size;
    float height;
    float range;
    float speed;
    UnitCreateFunc create_func;
    UnitAttackFunc attack_func;
    Mesh* icon_mesh;

    Animation* idle_animation;
    Animation* move_animation;
    Animation* shuffle_animation;
    Animation* attack_animation;
    Animation* reload_animation;
};

// @unit
extern void InitUnitInfo(const UnitInfo& unit_info);
extern void DrawGizmos(UnitEntity* u, const Mat3& transform);
extern UnitEntity* CreateUnit(UnitType type, Team team, const EntityVtable& vtable, const Vec3& position = VEC3_ZERO, float rotation=0.0f, const Vec2& scale=VEC2_ONE);
extern void EnumerateUnits(Team team, bool (*callback)(UnitEntity* unit, void* user_data), void* user_data);
extern void Damage(UnitEntity* u, DamageType damage_type, float amount);
extern UnitEntity* FindClosestEnemy(UnitEntity* unit, float max_distance = F32_MAX);
extern UnitEntity* FindClosestEnemy(Team team, const Vec3& position, float max_distance = F32_MAX);
extern UnitEntity* FindClosestUnit(const Vec3& position);
inline UnitEntity* GetUnit(const EntityHandle& handle) { return (UnitEntity*)GetEntity(handle); }
inline float Distance(UnitEntity* u1, UnitEntity* u2) { return Distance(XZ(u1->position), XZ(u2->position)); }
inline float Distance(UnitEntity* u, const Vec3& position) { return Distance(XZ(u->position), XZ(position)); }
inline float DistanceSqr(UnitEntity* u1, UnitEntity* u2) { return DistanceSqr(XZ(u1->position), XZ(u2->position)); }
inline float DistanceSqr(UnitEntity* u, const Vec3& position) { return DistanceSqr(XZ(u->position), XZ(position)); }
inline Vec3 Direction(UnitEntity* u1, const Vec3& position) { return XZ(Normalize(XZ(position) - XZ(u1->position))); }
inline Vec3 Direction(UnitEntity* u1, UnitEntity* u2) { return XZ(Normalize(XZ(u2->position) - XZ(u1->position))); }

// @team
inline Vec2 GetTeamDirection(Team team) {
    static Vec2 directions[TEAM_COUNT] = {
        Vec2{-1,0},
        Vec2{1,0}
    };
    return directions[team];
}

inline Vec2 GetTeamColorOffset(Team team) {
    static Vec2 color_offsets[TEAM_COUNT] = {
        Vec2{0.5f, 0.0f},
        Vec2{0.0f, 0.0f}
    };
    return color_offsets[team];
}

inline Color GetTeamColor(Team team) {
    static Color colors[TEAM_COUNT] = {
        Color32ToColor(228, 92, 95, 255),
        Color32ToColor(85, 177, 241, 255)
    };
    return colors[team];
}

inline Team GetOppositeTeam(Team team) {
    static Team opposite[TEAM_COUNT] = {
        TEAM_BLUE,
        TEAM_RED
    };

    return opposite[team];
}

extern void BindTeamColor(Team team);

// @unit
extern void SetState(UnitEntity* u, UnitState new_state);
extern Vec3 ComputeRVOVelocityForUnit(UnitEntity* u, const Vec3& preferred_velocity, float max_speed);
extern void ApplyImpulse(UnitEntity* u, const Vec3& impulse);
extern void UpdateUnit(UnitEntity* u);

// @stick
extern void DrawStick(Entity* e, const Mat3& transform, bool shadow);
extern void EnableRagdoll(Entity* entity);
extern void DisableRagdoll(Entity* entity);
extern void UpdateStickRagdoll(Entity* entity, float dt);

// @archer
extern ArcherEntity* CreateArcher(Team team, const Vec3& position);

// @cowboy
extern CowboyEntity* CreateCowboy(Team team, const Vec3& position);

// @knight
extern KnightEntity* CreateKnight(Team team, const Vec3& position);

// @tower
extern TowerEntity* CreateTower(Team team, const Vec3& position);

// @database
extern void InitUnitDatabase();
extern const UnitInfo* GetUnitInfo(UnitType unit_type);
