//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#pragma once

enum DamageType {
    DAMAGE_TYPE_PHYSICAL,
    DAMAGE_TYPE_COUNT
};

enum EntityType {
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_UNIT,
    ENTITY_TYPE_PROJECTILE,
    ENTITY_TYPE_COUNT
};

struct Entity;

struct EntityVtable {
    void (*update)(Entity* entity);
    void (*draw)(Entity* entity, const Mat3& transform);
    void (*draw_shadow)(Entity* entity, const Mat3& transform);
    void (*death)(Entity* entity, DamageType damage_type);
};

struct Entity {
    EntityType type;
    EntityVtable vtable;
    Vec3 position;
    Vec2 scale;
    float depth;
    float rotation;
    Animator animator;
};

#include "unit.h"
#include "projectile.h"

union FatEntity {
    Entity entity;
    FatUnitEntity unit;
    ProjectileEntity projectile;
};

// @entity
extern Entity* CreateEntity(EntityType type, const EntityVtable& vtable, const Vec3& position = VEC3_ZERO, float rotation=0.0f, const Vec2& scale=VEC2_ONE);
extern void UpdateAnimator(Entity* entity);
extern void DestroyAllEntities();
inline Vec2 WorldToScreen(const Vec3& pos) { return XY(pos) + Vec2{0.0f, pos.z}; }
