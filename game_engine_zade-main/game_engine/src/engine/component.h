#pragma once

#include "../vendor/lua/lua.hpp"
#include "../vendor/LuaBridge/LuaBridge.h"

#include <memory>
#include <string>

class Component
{
public:
	Component(lua_State* luaState, const std::string& typeName, const std::string& componentName);

	inline static void Log(const std::string& message) { std::cout << message << std::endl; }
	inline static void LogError(const std::string& message) { std::cerr << message << std::endl; }

	inline const bool getIsEnabled() const { return m_isEnabled; }
	inline const bool getHasStart() const { return m_hasStart; }
	inline const bool getHasUpdate() const { return m_hasLateUpdate; }
	inline const bool getHasLateUpdate() const { return m_isEnabled; }
	inline const std::shared_ptr<luabridge::LuaRef> getComponentReference() const { return m_reference; }
	inline const std::string getType() const { return m_type; }

public:
	bool m_isEnabled;
	bool m_hasStart;
	bool m_hasUpdate;
	bool m_hasLateUpdate;
	std::shared_ptr<luabridge::LuaRef> m_reference;
	std::string m_type;
};
