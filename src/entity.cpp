//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#include "game.h"

static u32 g_next_entity_generation = 1;

void UpdateAnimator(Entity* entity) {
    Update(entity->animator);
}

void DestroyAllEntities() {
    Clear(g_game.entity_allocator);
}

static void EntityDestructor(void* p) {
    Entity* e = &static_cast<FatEntity*>(p)->entity;
    e->generation = 0;
}

Entity* CreateEntity(EntityType type, const EntityVtable& vtable, const Vec3& position, float rotation, const Vec2& scale) {
    Entity* e = static_cast<Entity*>(Alloc(g_game.entity_allocator, sizeof(FatEntity), EntityDestructor));
    e->type = type;
    e->vtable = vtable;
    e->position = position;
    e->rotation = rotation;
    e->scale = scale;
    e->generation = g_next_entity_generation++;
    return e;
}

Entity* GetEntity(const EntityHandle& handle) {
    if (handle.index == 0)
        return nullptr;

    Entity* entity = static_cast<Entity*>(GetAt(g_game.entity_allocator, handle.index - 1));
    assert(entity);

    if (entity->generation != handle.generation)
        return nullptr;

    return entity;
}

extern EntityHandle GetHandle(Entity* entity) {
    if (!entity)
        return {};
    return EntityHandle{ GetIndex(g_game.entity_allocator, entity) + 1, entity->generation };
}
