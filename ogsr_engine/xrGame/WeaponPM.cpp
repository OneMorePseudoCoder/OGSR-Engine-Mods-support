#include "stdafx.h"
#include "WeaponPM.h"

CWeaponPM::CWeaponPM() : CWeaponPistol() {}

CWeaponPM::~CWeaponPM() {}

using namespace luabind;


void CWeaponPM::script_register(lua_State* L) { module(L)[class_<CWeaponPM, CGameObject>("CWeaponPM").def(constructor<>())]; }
