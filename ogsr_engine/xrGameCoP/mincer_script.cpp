#include "stdafx.h"
#include "Mincer.h"
#include "RadioactiveZone.h"

using namespace luabind;


void CMincer::script_register(lua_State* L)
{
    module(L)[class_<CMincer, CGameObject>("CMincer").def(constructor<>()), class_<CRadioactiveZone, CGameObject>("CRadioactiveZone").def(constructor<>())];
}
