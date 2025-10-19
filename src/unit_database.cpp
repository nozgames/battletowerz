//
//  Battle TowerZ - Copyright(c) 2025 NoZ Games, LLC
//

static UnitInfo g_unit_database[UNIT_TYPE_COUNT] = {};

const UnitInfo* GetUnitInfo(UnitType unit_type) {
    if (unit_type == UNIT_TYPE_UNKNOWN)
        return nullptr;

    assert(unit_type < UNIT_TYPE_COUNT);
    return &g_unit_database[unit_type];
}

static void InitUnitInfo(
    UnitType unit_type,
    const char* name_str,
    UnitCreateFunc create_func) {
    g_unit_database[unit_type] = {
        .type = unit_type,
        .name = GetName(name_str),
        .create_func = create_func
    };
}

void InitUnitDatabase() {
    InitUnitInfo(UNIT_TYPE_TOWER, "Tower", (UnitCreateFunc)CreateTower);
    InitUnitInfo(UNIT_TYPE_ARCHER, "Archer", (UnitCreateFunc)CreateArcher);
    InitUnitInfo(UNIT_TYPE_KNIGHT, "Knight", (UnitCreateFunc)CreateKnight);
}
