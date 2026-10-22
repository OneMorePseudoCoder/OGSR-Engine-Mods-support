#include "stdafx.h"
#include "WeaponVintorez.h"

CWeaponVintorez::CWeaponVintorez(void) : CWeaponMagazined(SOUND_TYPE_WEAPON_SNIPERRIFLE) {}

CWeaponVintorez::~CWeaponVintorez(void) {}

using namespace luabind;


void CWeaponVintorez::script_register(lua_State* L) { module(L)[class_<CWeaponVintorez, CGameObject>("CWeaponVintorez").def(constructor<>())]; }
