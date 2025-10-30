//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#pragma once

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
    UNIT_STATE_MOVING,
    UNIT_STATE_ATTACKING,
    UNIT_STATE_DEAD,
    UNIT_STATE_COUNT
};

struct UnitEntity : Entity {
    UnitType unit_type;
    UnitState state;
    Team team;
    float health;
    float size;
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

typedef UnitEntity* (*UnitCreateFunc)(Team team, const Vec2& position);

struct UnitInfo {
    UnitType type;
    const Name* name;
    float size;
    UnitCreateFunc create_func;
    Mesh* icon_mesh;
};

// @unit
extern void InitUnitInfo(const UnitInfo& unit_info);

extern UnitEntity* CreateUnit(UnitType type, Team team, const EntityVtable& vtable, const Vec3& position = VEC3_ZERO, float rotation=0.0f, const Vec2& scale=VEC2_ONE);
extern void EnumerateUnits(Team team, bool (*callback)(UnitEntity* unit, void* user_data), void* user_data);
extern void Damage(UnitEntity* u, DamageType damage_type, float amount);
extern UnitEntity* FindClosestEnemy(UnitEntity* unit);
extern UnitEntity* FindClosestUnit(const Vec2& position);
extern void MoveTowards(UnitEntity* unit, const Vec2& target_position, float speed);

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
