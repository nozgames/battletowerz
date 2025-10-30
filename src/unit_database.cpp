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

void InitUnitInfo(const UnitInfo& unit_info) {
    g_unit_database[unit_info.type] = unit_info;
}

extern void InitCowboyUnit();
extern void InitArcherUnit();

void InitUnitDatabase() {
    InitCowboyUnit();
    InitArcherUnit();
}
