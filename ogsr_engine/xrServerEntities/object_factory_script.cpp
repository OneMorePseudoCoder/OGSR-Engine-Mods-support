////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_script.cpp
//	Created 	: 27.05.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "object_factory.h"
#include "ai_space.h"
#include "script_engine.h"
#include "object_item_script.h"

void CObjectFactory::register_script_class(LPCSTR client_class, LPCSTR server_class, LPCSTR clsid, LPCSTR script_clsid)
{
    luabind::object client;
    if (!ai().script_engine().function_object(client_class, client, LUA_TUSERDATA))
    {
        ai().script_engine().script_log(eLuaMessageTypeError, "Cannot register class %s", client_class);
        return;
    }
    luabind::object server;
    if (!ai().script_engine().function_object(server_class, server, LUA_TUSERDATA))
    {
        ai().script_engine().script_log(eLuaMessageTypeError, "Cannot register class %s", server_class);
        return;
    }

    add(xr_new<CObjectItemScript>(client, server, TEXT2CLSID(clsid), script_clsid));
}

void CObjectFactory::register_script_class(LPCSTR unknown_class, LPCSTR clsid, LPCSTR script_clsid)
{
    luabind::object creator;
    if (!ai().script_engine().function_object(unknown_class, creator, LUA_TUSERDATA))
    {
        ai().script_engine().script_log(eLuaMessageTypeError, "Cannot register class %s", unknown_class);
        return;
    }
    add(xr_new<CObjectItemScript>(creator, creator, TEXT2CLSID(clsid), script_clsid));
}


void CObjectFactory::register_script_classes()
{
    ai();
}

using namespace luabind;

struct CInternal
{};

void CObjectFactory::register_script() const
{
    actualize();

    luabind::class_<CInternal> instance("clsid");

    auto B = clsids().begin();
    auto E = clsids().end();

    for (auto const_it = B; const_it != E; ++const_it)
    {
        instance = std::move(instance).enum_("_clsid")[luabind::value(*(*const_it)->script_clsid(), int(const_it - B))];
    }

    lua_State* L = ai().script_engine().lua();
    luabind::module(L)[std::move(instance)]; // это представление нельзя обработать как таблицу

    lua_newtable(L);
    for (auto const_it = B; const_it != E; ++const_it)
    {
        lua_pushinteger(L, int(const_it - B));
        lua_setfield(L, -2, *(*const_it)->script_clsid());
    }

    lua_setglobal(L, "clsid_table"); // это представление можно обработать как таблицу :)
}


void CObjectFactory::script_register(lua_State* L)
{
    module(L)[class_<CObjectFactory>("object_factory")
                  .def("register", (void (CObjectFactory::*)(LPCSTR, LPCSTR, LPCSTR, LPCSTR))(&CObjectFactory::register_script_class))
                  .def("register", (void (CObjectFactory::*)(LPCSTR, LPCSTR, LPCSTR))(&CObjectFactory::register_script_class))];
}