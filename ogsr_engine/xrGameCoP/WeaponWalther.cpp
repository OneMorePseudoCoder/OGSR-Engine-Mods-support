#include "stdafx.h"
#include "WeaponWalther.h"

CWeaponWalther::CWeaponWalther(void) {}

CWeaponWalther::~CWeaponWalther(void) {}

using namespace luabind;


void CWeaponWalther::script_register(lua_State* L) { module(L)[class_<CWeaponWalther, CGameObject>("CWeaponWalther").def(constructor<>())]; }
