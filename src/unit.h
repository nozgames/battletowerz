//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#pragma once

enum Team {
    TEAM_RED,
    TEAM_BLUE,
    TEAM_COUNT
};

enum UnitType {
    UNIT_TYPE_ARCHER,
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

enum DamageType {
    DAMAGE_TYPE_PHYSICAL,
    DAMAGE_TYPE_COUNT
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

struct TowerEntity : UnitEntity {
};

union FatUnitEntity {
    UnitEntity unit;
    ArcherEntity archer;
    KnightEntity knight;
    TowerEntity tower;
};

// @unit
extern UnitEntity* CreateUnit(UnitType type, Team team, const EntityVtable& vtable, const Vec2& position = VEC2_ZERO, float rotation=0.0f, const Vec2& scale=VEC2_ONE);
extern void EnumerateUnits(Team team, bool (*callback)(UnitEntity* unit, void* user_data), void* user_data);
extern void Damage(UnitEntity* u, DamageType damage_type, float amount);

// @team
inline Vec2 GetTeamDirection(Team team) {
    static Vec2 directions[TEAM_COUNT] = {
        Vec2{-1,0},
        Vec2{1,0}
    };
    return directions[team];
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
extern ArcherEntity* CreateArcher(Team team, const Vec2& position);

// @knight
extern KnightEntity* CreateKnight(Team team, const Vec2& position);

// @tower
extern TowerEntity* CreateTower(Team team, const Vec2& position);
