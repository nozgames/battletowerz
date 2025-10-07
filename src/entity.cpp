//
//  NockerZ - Copyright(c) 2025 NoZ Games, LLC
//

#include "game.h"

void UpdateAnimator(Entity* entity)
{
    Update(entity->animator);
}

Entity* CreateEntity(EntityType type, const EntityVtable& vtable, const Vec2& position, float rotation, const Vec2& scale)
{
    Entity* e = static_cast<Entity*>(Alloc(g_game.entity_allocator, sizeof(FatEntity)));
    e->type = type;
    e->vtable = vtable;
    e->position = position;
    e->rotation = rotation;
    e->scale = scale;
    return e;
}

