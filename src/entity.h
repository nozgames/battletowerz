//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#pragma once

enum EntityType
{
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_UNIT,
    ENTITY_TYPE_COUNT
};

struct Entity;

struct EntityVtable
{
    void (*update)(Entity* entity);
    void (*render)(Entity* entity, const Mat3& transform);
};

struct Entity {
    EntityType type;
    EntityVtable vtable;
    Vec2 position;
    Vec2 scale;
    float depth;
    float rotation;
    Animator animator;
};

#include "unit.h"

union FatEntity
{
    Entity entity;
    FatUnitEntity unit;
};
