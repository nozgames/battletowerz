//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#pragma once

enum ProjectileType
{
    PROJECTILE_TYPE_NONE,
    PROJECTILE_TYPE_ARROW,
    PROJECTILE_TYPE_BULLET,
    PROJECTILE_TYPE_COUNT
};

struct ProjectileEntity : Entity
{
    ProjectileType projectile_type;
    Team team;
    Vec3 velocity;
    float damage;
    float height;
    float time;
    float elapsed;
    float distance;
    float speed;
    Vec2 start;
    Vec2 target;
    Vec3 apex;
    Vec3 last_position;
};

// @projectile
extern ProjectileEntity* CreateProjectile(ProjectileType type, Team team, const EntityVtable& vtable, const Vec3& position, const Vec3& velocity, const Vec2& scale);

// @arrow
extern ProjectileEntity* CreateArrow(Team team, const Vec3& position, const Vec2& target, float speed);

// @bullet
extern ProjectileEntity* CreateBullet(Team team, const Vec3& position, const Vec2& target, float speed);


