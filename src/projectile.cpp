//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

ProjectileEntity* CreateProjectile(ProjectileType type, Team team, const EntityVtable& vtable, const Vec3& position, const Vec3& velocity, const Vec2& scale) {
    ProjectileEntity* p = static_cast<ProjectileEntity*>(CreateEntity(ENTITY_TYPE_PROJECTILE, vtable, position, 0.0f, scale));
    p->projectile_type = type;
    p->team = team;
    p->velocity = velocity;
    return p;
}