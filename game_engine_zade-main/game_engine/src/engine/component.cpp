#include "component.h"

#include "../vendor/lua/lua.hpp"
#include "../vendor/LuaBridge/LuaBridge.h"

Component::Component(lua_State* luaState, const std::string& typeName, const std::string& componentKey)
    : m_isEnabled(true), m_hasStart(true), m_hasUpdate(false), m_hasLateUpdate(false)
{
    // Find global component table first
    luabridge::LuaRef componentTable = luabridge::getGlobal(luaState, typeName.c_str());

    // Create metatable for instance
    luabridge::LuaRef metaTable = luabridge::newTable(luaState);
    metaTable["__index"] = componentTable;

    // Create and initialize instance table
    luabridge::LuaRef instanceTable = luabridge::newTable(luaState);

    // Set the "key" field inside our instanceTable
    instanceTable["key"] = componentKey;

    instanceTable["type"] = typeName;

    // Set the metatable for our instance table
    instanceTable.push(luaState);
    metaTable.push(luaState);
    lua_setmetatable(luaState, -2);
    lua_pop(luaState, 1);

    m_reference = std::make_shared<luabridge::LuaRef>(instanceTable);
    m_type = typeName;

    // More initialization if needed...
}
