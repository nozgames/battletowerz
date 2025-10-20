//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

inline ProjectileEntity* CastArrow(Entity* e) {
    assert(e && e->type == ENTITY_TYPE_PROJECTILE);
    ProjectileEntity* p = static_cast<ProjectileEntity*>(e);
    assert(p->projectile_type == PROJECTILE_TYPE_ARROW);
    return p;
}

static void RenderArrow(Entity* e, const Mat3& transform) {
    ProjectileEntity* p = CastArrow(e);

    BindDepth(1.0f);
    BindMaterial(g_game.material);
    BindColor(GetTeamColor(p->team));
    DrawMesh(MESH_PROJECTILE_ARROW, transform * Scale(1.0f + (e->position.z / 10.0f)));
    BindDepth(0.0f);

    BindDepth(-7.0f);
    BindColor({0,0,0,0.05f});
    BindMaterial(g_game.shadow_material);
    DrawMesh(MESH_PROJECTILE_ARROW, TRS(XY(e->position), Angle(Normalize(p->target - p->start)), e->scale) * Scale(1.0f + (e->position.z / 10.0f)));
}

static void UpdateArrow(Entity* e) {
    (void)e;
    ProjectileEntity* p = CastArrow(e);
    float dt = GetGameFrameTime();
    p->elapsed += dt;
    p->velocity.z += GRAVITY * dt;
    p->position += p->velocity * dt;

    float nt = p->elapsed / p->time;
    if (nt >= 1.0f) {
        Free(p);
        return;
    }

    p->rotation = Angle(Normalize(WorldToScreen(p->position - p->last_position)));
    p->last_position = p->position;
}

static void CalculateTrajectoryWithGravity(ProjectileEntity* p, const Vec2& target, float speed) {
    Vec2 to_target = target - XY(p->position);
    float distance = Length(to_target);
    Vec2 direction = Normalize(to_target);
    float time = distance / speed;
    float vz = -(GRAVITY * time * 0.5f);
    p->velocity = Vec3{direction.x * speed, direction.y * speed, vz};
    p->apex = Vec3{direction.x * speed * 0.5f, direction.y * speed * 0.5f, vz};
    p->time = time;
    p->speed = speed;
    p->elapsed = 0.0f;
}

ProjectileEntity* CreateArrow(Team team, const Vec3& position, const Vec2& target, float speed) {
    static EntityVtable vtable = {
        .update = UpdateArrow,
        .render = RenderArrow
    };

    ProjectileEntity* e = CreateProjectile(PROJECTILE_TYPE_ARROW, team, vtable, position, VEC3_ZERO, VEC2_ONE);
    e->start = XY(position);
    e->target = target;
    e->last_position = position;
    CalculateTrajectoryWithGravity(e, target, speed);
    return e;
}
