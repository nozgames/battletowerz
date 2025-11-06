//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

// Simple ragdoll bone structure
struct RagdollBone {
    Vec2 position;
    Vec2 offset;
    Vec2 velocity;
    float rotation;
    float angular_velocity;
    int parent_index;
    float length;
    bool is_root;
};

// Ragdoll state
struct StickRagdoll {
    RagdollBone bones[BONE_STICK_COUNT]; // One for each stick bone
    bool active;
    float ground_y;
};

// Simple ragdoll physics constants
constexpr float RAGDOLL_GRAVITY = 98.0f;
constexpr float RAGDOLL_DAMPING = 0.98f;
constexpr float RAGDOLL_ANGULAR_DAMPING = 0.95f;
constexpr float RAGDOLL_BOUNCE = 0.3f;
constexpr float RAGDOLL_FRICTION = 40.0f;
constexpr float RAGDOLL_EXPLODE_VELOCITY_X = 5.0f;
constexpr float RAGDOLL_EXPLODE_VELOCITY_Y = 20.0f;

// Initialize ragdoll for a stick figure
static void InitRagdoll(StickRagdoll& ragdoll, Entity* entity) {
    ragdoll.active = true;
    ragdoll.ground_y = entity->position.y;

    // Stop(entity->animator);
    // Play(entity->animator, ANIMATION_STICK_DEAD, 1.0f, false);
    // Update(entity->animator, 1.0f);

    Animator test = {};
    Init(test, SKELETON_STICK);
    Play(test, ANIMATION_STICK_DEAD, 1.0f, false);
    Update(test, 1.0f);

    float h = 0.4f;

    // Initialize bones from current animator pose
    Skeleton* skeleton = SKELETON_STICK;
    for (int bone_index = 0; bone_index < BONE_STICK_COUNT; bone_index++) {
        const Bone& bone = GetBone(skeleton, bone_index);
        const Transform& bone_transform = GetBoneTransform(skeleton, bone_index);

        RagdollBone& ragdoll_bone = ragdoll.bones[bone_index];
        ragdoll_bone.parent_index = bone.parent_index;
        ragdoll_bone.length = Length(bone_transform.position);
        ragdoll_bone.is_root = bone_index == 0;

        Vec2 bone_pos = TransformPoint(entity->animator.bones[bone_index]);
        ragdoll_bone.position = bone_pos;
        ragdoll_bone.rotation = 0.0f;
        //ragdoll_bone.velocity = Vec2{RandomFloat(RAGDOLL_EXPLODE_VELOCITY_X * 0.5f, RAGDOLL_EXPLODE_VELOCITY_X), RandomFloat(RAGDOLL_EXPLODE_VELOCITY_Y * 0.5f, RAGDOLL_EXPLODE_VELOCITY_Y)};

        ragdoll_bone.velocity = Vec2{
            (1.0f - Abs(bone_pos.y - h) / h) * RAGDOLL_EXPLODE_VELOCITY_X,
            ((bone_pos.y - h) / h) * RAGDOLL_EXPLODE_VELOCITY_Y};

        //ragdoll_bone.angular_velocity = RandomFloat(-5.0f, 5.0f);

        if (bone_index > 0) {
            Vec2 b = TransformPoint(test.bones[bone_index], Vec2{1,0});
            Vec2 bp = TransformPoint(test.bones[bone_index]);
            Vec2 dir = Normalize(b - bp);
            ragdoll_bone.rotation = Angle(dir); //   SignedAngleDelta(bone.transform.rotation, Angle(dir));
            ragdoll_bone.offset = {0, bp.y};
        }
        // Extract position and rotation from current bone transform
    }
}

// Update ragdoll physics
static void UpdateRagdoll(StickRagdoll& ragdoll, float dt) {
    if (!ragdoll.active) return;

    for (int i = 0; i < BONE_STICK_COUNT; i++) {
        RagdollBone& bone = ragdoll.bones[i];
        bone.velocity.y -= RAGDOLL_GRAVITY * dt;
        //bone.velocity *= RAGDOLL_DAMPING;
        //bone.angular_velocity *= RAGDOLL_ANGULAR_DAMPING;
        bone.position += bone.velocity * dt;
        //bone.rotation += bone.angular_velocity * dt;

        //bone.position = Normalize(bone.position) * bone.length;

        // Ground collision
        if (bone.position.y <= ragdoll.ground_y) {
            bone.position.y = ragdoll.ground_y;
            bone.velocity.y = -bone.velocity.y * RAGDOLL_BOUNCE;

            // Stop bouncing if velocity is very small4
            if (Abs(bone.velocity.y) < 0.5f) {
                bone.velocity.y = 0.0f;

                if (bone.velocity.x != 0.0f) {
                    float old_x_velocity = bone.velocity.x;
                    bone.velocity.x -= RAGDOLL_FRICTION * dt;
                    if (old_x_velocity * bone.velocity.x < 0.0f) {
                        bone.velocity.x = 0.0f;
                    }
                }
            }
        }
    }
}

static void ApplyRagdollToAnimator(StickRagdoll& ragdoll, Animator& animator) {
    if (!ragdoll.active) return;

    for (int i = 0; i < BONE_STICK_COUNT; i++) {
        const RagdollBone& ragdoll_bone = ragdoll.bones[i];
        animator.bones[i] = TRS(ragdoll_bone.position + ragdoll_bone.offset, ragdoll_bone.rotation, VEC2_ONE);
    }
}

void DrawStick(Entity* e, const Mat3& transform, bool ) {
    DrawMesh(MESH_STICK_ARM_L_R, transform, e->animator, BONE_STICK_ARM_LOWER_B);
    DrawMesh(MESH_STICK_ARM_U_R, transform, e->animator, BONE_STICK_ARM_UPPER_B);
    DrawMesh(MESH_STICK_ARM_L_L, transform, e->animator, BONE_STICK_ARM_LOWER_F);
    DrawMesh(MESH_STICK_ARM_U_L, transform, e->animator, BONE_STICK_ARM_UPPER_F);
    DrawMesh(MESH_STICK_HAND_R, transform, e->animator, BONE_STICK_HAND_B);
    DrawMesh(MESH_STICK_HAND_L, transform, e->animator, BONE_STICK_HAND_F);
    DrawMesh(MESH_STICK_LEG_U_R, transform, e->animator, BONE_STICK_LEG_UPPER_B);
    DrawMesh(MESH_STICK_LEG_L_R, transform, e->animator, BONE_STICK_LEG_LOWER_B);
    DrawMesh(MESH_STICK_LEG_L_L, transform, e->animator, BONE_STICK_LEG_LOWER_F);
    DrawMesh(MESH_STICK_LEG_U_L, transform, e->animator, BONE_STICK_LEG_UPPER_F);
    DrawMesh(MESH_STICK_HIP, transform, e->animator, BONE_STICK_HIP);
    DrawMesh(MESH_STICK_BODY_B, transform, e->animator, BONE_STICK_SPINE);
    DrawMesh(MESH_STICK_BODY, transform, e->animator, BONE_STICK_CHEST);
    DrawMesh(MESH_STICK_NECK, transform, e->animator, BONE_STICK_NECK);
    DrawMesh(MESH_STICK_HEAD, transform, e->animator, BONE_STICK_HEAD);
    // DrawMesh(e->health <= 0 ? MESH_STICK_EYE_DEAD : MESH_STICK_EYE, transform, e->animator, BONE_STICK_EYE_F);
    // DrawMesh(e->health <= 0 ? MESH_STICK_EYE_DEAD : MESH_STICK_EYE, transform, e->animator, BONE_STICK_EYE_B);
}

void BindTeamColor(Team team) {
    BindColor(COLOR_WHITE, GetTeamColorOffset(team));
}

// Public ragdoll API
// Store ragdoll per entity (simple global for now, could be extended)
static StickRagdoll g_entity_ragdolls[1024];
static bool g_entity_has_ragdoll[1024];

void EnableRagdoll(Entity* entity) {
    size_t index = GetIndex(g_game.entity_allocator, entity);
    if (index >= 1024) return;

    if (!g_entity_has_ragdoll[index]) {
        InitRagdoll(g_entity_ragdolls[index], entity);
        g_entity_has_ragdoll[index] = true;
    }
}

void DisableRagdoll(Entity* entity) {
    size_t index = GetIndex(g_game.entity_allocator, entity);
    if (index >= 1024) return;

    g_entity_ragdolls[index].active = false;
    g_entity_has_ragdoll[index] = false;
}

void UpdateStickRagdoll(Entity* entity, float dt) {
    size_t index = GetIndex(g_game.entity_allocator, entity);
    if (index >= 1024) return;

    if (g_entity_has_ragdoll[index]) {
        UpdateRagdoll(g_entity_ragdolls[index], dt);
        ApplyRagdollToAnimator(g_entity_ragdolls[index], entity->animator);
    }
}
