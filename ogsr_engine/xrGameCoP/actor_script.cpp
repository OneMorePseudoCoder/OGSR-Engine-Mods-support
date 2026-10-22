////////////////////////////////////////////////////////////////////////////
//	Module 		: actor_script.cpp
//	Created 	: 17.01.2008
//  Modified 	: 17.01.2008
//	Author		: Dmitriy Iassenev
//	Description : actor script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Actor.h"
#include "level_changer.h"

using namespace luabind;

bool isGodMode() { return psActorFlags.test(AF_GODMODE); }


void CActor::script_register(lua_State* L)
{
    module(L)[class_<CActor, CGameObject>("CActor").def(constructor<>()), class_<CLevelChanger, CGameObject>("CLevelChanger").def(constructor<>()), def("is_god_mode", isGodMode)];
}
