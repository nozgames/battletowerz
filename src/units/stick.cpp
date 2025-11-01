//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

#if 0
static void DrawStickBackArm(Entity* e, const Mat3& transform, bool ) {
    DrawMesh(MESH_STICK_ARM_L_R, transform, e->animator, BONE_STICK_ARM_LOWER_B);
    DrawMesh(MESH_STICK_ARM_U_R, transform, e->animator, BONE_STICK_ARM_UPPER_B);
    DrawMesh(MESH_STICK_REVOLVER, transform, e->animator, BONE_STICK_ITEM_B);
    DrawMesh(MESH_STICK_HAND_R, transform, e->animator, BONE_STICK_HAND_B);
}

static void DrawStickFrontArm(Entity* e, const Mat3& transform, bool ) {
    DrawMesh(MESH_STICK_ARM_L_L, transform, e->animator, BONE_STICK_ARM_LOWER_F);
    DrawMesh(MESH_STICK_ARM_U_L, transform, e->animator, BONE_STICK_ARM_UPPER_F);
    DrawMesh(MESH_STICK_HAND_L, transform, e->animator, BONE_STICK_HAND_F);
}

static void DrawStickBackLeg(Entity* e, const Mat3& transform, bool) {
    DrawMesh(MESH_STICK_LEG_U_R, transform, e->animator, BONE_STICK_LEG_UPPER_B);
    DrawMesh(MESH_STICK_LEG_L_R, transform, e->animator, BONE_STICK_LEG_LOWER_B);
}

static void DrawStickEyes(UnitEntity* e, const Mat3& transform, bool) {
    DrawMesh(e->health <= 0 ? MESH_STICK_EYE_DEAD : MESH_STICK_EYE, transform, e->animator, BONE_STICK_EYE_F);
    DrawMesh(e->health <= 0 ? MESH_STICK_EYE_DEAD : MESH_STICK_EYE, transform, e->animator, BONE_STICK_EYE_B);
}

static void DrawStickBody(Entity* e, const Mat3& transform, bool ) {
    DrawMesh(MESH_STICK_HIP, transform, e->animator, BONE_STICK_HIP);
    DrawMesh(MESH_STICK_BODY_B, transform, e->animator, BONE_STICK_SPINE);
    DrawMesh(MESH_STICK_BODY, transform, e->animator, BONE_STICK_CHEST);
    DrawMesh(MESH_STICK_LEG_L_L, transform, e->animator, BONE_STICK_LEG_LOWER_F);
    DrawMesh(MESH_STICK_LEG_U_L, transform, e->animator, BONE_STICK_LEG_UPPER_F);
    DrawMesh(MESH_STICK_NECK, transform, e->animator, BONE_STICK_NECK);
    DrawMesh(MESH_STICK_HEAD, transform, e->animator, BONE_STICK_HEAD);
}

static void BindColor(UnitEntity* u) {
    BindColor(COLOR_WHITE, GetTeamColorOffset(u->team));
}

static void BindOutlineColor(UnitEntity* u) {
    BindColor(COLOR_WHITE, GetTeamColorOffset(u->team) + Vec2{0.0f, 1.0f/16.0f});
}


#endif
