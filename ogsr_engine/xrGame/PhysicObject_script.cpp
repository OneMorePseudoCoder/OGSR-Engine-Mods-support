#include "stdafx.h"
#include "PhysicObject.h"
#include "PHDestroyable.h"

using namespace luabind;

void CPhysicObject::script_register(lua_State* L) 
{ 
	module(L)
	[
		class_<CPhysicObject, CGameObject>("CPhysicObject")
		.def(constructor<>())
		.def("set_ai_obstacle", &CPhysicObject::set_ai_obstacle)
		.def("set_door_ignore_dynamics", &CPhysicObject::set_door_ignore_dynamics)
		.def("unset_door_ignore_dynamics", &CPhysicObject::unset_door_ignore_dynamics)
	]; 
}
