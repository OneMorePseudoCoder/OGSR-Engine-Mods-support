#include "stdafx.h"
#include "WeaponUSP45.h"

CWeaponUSP45::CWeaponUSP45() {}

CWeaponUSP45::~CWeaponUSP45() {}

using namespace luabind;


void CWeaponUSP45::script_register(lua_State* L) { module(L)[class_<CWeaponUSP45, CGameObject>("CWeaponUSP45").def(constructor<>())]; }
