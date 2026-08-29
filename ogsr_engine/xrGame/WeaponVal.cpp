#include "stdafx.h"
#include "WeaponVal.h"

CWeaponVal::CWeaponVal(void) : CWeaponMagazined(SOUND_TYPE_WEAPON_SUBMACHINEGUN) {}

CWeaponVal::~CWeaponVal(void) {}

using namespace luabind;


void CWeaponVal::script_register(lua_State* L) { module(L)[class_<CWeaponVal, CGameObject>("CWeaponVal").def(constructor<>())]; }
