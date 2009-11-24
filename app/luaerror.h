#ifndef LUAERROR_H
#define LUAERROR_H

#include <QSharedPointer>

#include <iostream>
#include <stdexcept>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lstate.h"
}


class LuaError : public std::exception
{
private:
    lua_State * m_L;
    // resource for error object on Lua stack (is to be popped
    // when no longer used)
    QSharedPointer<lua_State> m_lua_resource;
    LuaError & operator=(const LuaError & other); // prevent
public:
    // Construct using top-most element on Lua stack as error.
    LuaError(lua_State * L);
    LuaError(const LuaError & other);
    ~LuaError() throw();
    virtual const char * what() const throw();
};

inline static void LuaError_lua_resource_delete(lua_State * L)
{
    lua_pop(L, 1);
}

inline LuaError::LuaError(lua_State * L)
    : m_L(L), m_lua_resource(L, LuaError_lua_resource_delete)
{

}

inline LuaError::LuaError(const LuaError & other)
    : std::exception(other), m_L(other.m_L), m_lua_resource(other.m_lua_resource)
{

}
inline const char *LuaError::what() const throw()
{
    const char * s = lua_tostring(m_L, -1);
    if (s == NULL) s = "unrecognized Lua error";
    return s;
}
inline LuaError::~LuaError() throw()
{
}


#endif // LUAERROR_H
