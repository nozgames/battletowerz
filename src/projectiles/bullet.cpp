//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

inline ProjectileEntity* CastBullet(Entity* e) {
    assert(e && e->type == ENTITY_TYPE_PROJECTILE);
    ProjectileEntity* p = static_cast<ProjectileEntity*>(e);
    assert(p->projectile_type == PROJECTILE_TYPE_BULLET);
    return p;
}

static void DrawBullet(Entity*, const Mat3& transform) {
    BindDepth(1.0f);
    BindMaterial(g_game.material);
    BindColor(COLOR_WHITE);
    DrawMesh(MESH_BULLET, transform);
    BindDepth(0.0f);
}

static void UpdateBullet(Entity* e) {
    (void)e;
    ProjectileEntity* p = CastBullet(e);
    p->position += p->velocity * GetGameFrameTime();
    p->last_position = p->position;
}

ProjectileEntity* CreateBullet(Team team, const Vec3& position, const Vec2& target, float speed) {
    static EntityVtable vtable = {
        .update = UpdateBullet,
        .draw = DrawBullet
    };

    ProjectileEntity* e = CreateProjectile(PROJECTILE_TYPE_BULLET, team, vtable, position, VEC3_ZERO, VEC2_ONE);
    e->start = XY(position);
    e->target = target;
    e->last_position = position;
    e->velocity = Normalize(Vec3{target.x - position.x, target.y - position.y, 0.0f}) * speed;
    e->rotation = Angle(Normalize(WorldToScreen(e->velocity)));
    return e;
}
