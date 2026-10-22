#include "stdafx.h"
#include "StalkerOutfit.h"
#include "ActorHelmet.h"

CStalkerOutfit::CStalkerOutfit() {}

CStalkerOutfit::~CStalkerOutfit() {}

using namespace luabind;


void CStalkerOutfit::script_register(lua_State* L)
{
    module(L)[class_<CStalkerOutfit, CGameObject>("CStalkerOutfit").def(constructor<>()),

              class_<CHelmet, CGameObject>("CHelmet").def(constructor<>())];
}
