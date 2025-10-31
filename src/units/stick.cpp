//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

static void DrawStickRightArm(Entity* e, const Mat3& transform, bool ) {
    DrawMesh(MESH_STICK_ARM_L_R, transform, e->animator, BONE_STICK_ARM_L_R);
    DrawMesh(MESH_STICK_ARM_U_R, transform, e->animator, BONE_STICK_ARM_U_R);
    DrawMesh(MESH_STICK_REVOLVER, transform, e->animator, BONE_STICK_ITEM_R);
    DrawMesh(MESH_STICK_HAND_R, transform, e->animator, BONE_STICK_HAND_R);
}

static void DrawStickLeftArm(Entity* e, const Mat3& transform, bool ) {
    DrawMesh(MESH_STICK_ARM_L_L, transform, e->animator, BONE_STICK_ARM_L_L);
    DrawMesh(MESH_STICK_ARM_U_L, transform, e->animator, BONE_STICK_ARM_U_L);
    DrawMesh(MESH_STICK_HAND_L, transform, e->animator, BONE_STICK_HAND_L);
}

static void DrawStickBackLeg(Entity* e, const Mat3& transform, bool) {
    DrawMesh(MESH_STICK_LEG_L, transform, e->animator, BONE_STICK_LEG_L_R);
    DrawMesh(MESH_STICK_LEG_U, transform, e->animator, BONE_STICK_LEG_U_R);
    DrawMesh(MESH_STICK_BOOT_SPUR, transform, e->animator, BONE_STICK_LEG_L_R);
}

static void DrawStickEyes(UnitEntity* e, const Mat3& transform, bool) {
    DrawMesh(e->health <= 0 ? MESH_STICK_EYE_DEAD : MESH_STICK_EYE, transform, e->animator, BONE_STICK_EYE_L);
    DrawMesh(e->health <= 0 ? MESH_STICK_EYE_DEAD : MESH_STICK_EYE, transform, e->animator, BONE_STICK_EYE_R);
}

static void DrawStickBody(Entity* e, const Mat3& transform, bool ) {
    DrawMesh(MESH_STICK_HIP, transform, e->animator, BONE_STICK_HIP);
    DrawMesh(MESH_STICK_BODY, transform, e->animator, BONE_STICK_SPINE_B);
    DrawMesh(MESH_STICK_BODY_B, transform, e->animator, BONE_STICK_SPINE_A);
    DrawMesh(MESH_STICK_LEG_L, transform, e->animator, BONE_STICK_LEG_L_L);
    DrawMesh(MESH_STICK_LEG_U, transform, e->animator, BONE_STICK_LEG_U_L);
    DrawMesh(MESH_STICK_BOOT_SPUR, transform, e->animator, BONE_STICK_LEG_L_L);
    DrawMesh(MESH_STICK_NECK, transform, e->animator, BONE_STICK_NECK);
    DrawMesh(MESH_STICK_HEAD, transform, e->animator, BONE_STICK_HEAD);
}

static void BindColor(UnitEntity* u) {
    BindColor(COLOR_WHITE, GetTeamColorOffset(u->team));
}

static void BindOutlineColor(UnitEntity* u) {
    BindColor(COLOR_WHITE, GetTeamColorOffset(u->team) + Vec2{0.0f, 1.0f/16.0f});
}


