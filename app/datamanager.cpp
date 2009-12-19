#include "datamanager.h"

#include "luaerror.h"
#include <assert.h>

void DataManager::setKeyframeFromRef(int keyframeRef, float value)
{
    assert(NULL != m_L);

    struct C
    {
        int keyRef;
        float value;
        static int call(lua_State* L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, 1));
            lua_rawgeti(L, LUA_REGISTRYINDEX, p->keyRef);
            lua_pushnumber(L, p->value);
            lua_setfield(L, -2, "value");
            lua_pop(L, 1); /* << keyref */
            return 0;
        }
    } p = { keyframeRef, value };
    try
    {
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() <<std::endl;
    }
}

int DataManager::addKeyframe(int avarRef, int index, float value)
{
    assert(NULL != m_L);

    struct C
    {
        int avarRef;
        int frame;
        float value;
        int keyRef;
        static int call(lua_State* L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, 1));
            lua_rawgeti(L, LUA_REGISTRYINDEX, p->avarRef);      // Avar
            lua_getfield(L, -1, "addKeyframe");                 // Avar - addKeyframe
            lua_pushvalue(L, -2);   // self                     // Avar - addKeyframe - Avar
            lua_pushinteger(L, p->frame);                       // Avar - addKeyframe - Avar - frame
            lua_pushnumber(L, p->value);                        // Avar - addKeyframe - Avar - frame - value
            lua_call(L, 3, 1);                                  // Avar - kf
            p->keyRef = luaL_ref(L, LUA_REGISTRYINDEX);         // Avar
            lua_pop(L, 1);                                      // --
            return 0;
        }
    } p = { avarRef, index, value, 0 };
    try
    {
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() <<std::endl;
    }
    return p.keyRef;
}

float DataManager::getKeyframe(int keyframeRef)
{
    try
    {
        struct C
        {
            int keyRef;
            float value;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, 1));
                lua_rawgeti(L, LUA_REGISTRYINDEX, p->keyRef);
                lua_getfield(L, -1, "value");
                p->value = lua_tonumber(L, -1);
                lua_pop(L, 2); /* << value << keyref */
                return 0;
            }
        } p = { keyframeRef, 0.0f };
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
        return p.value;
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() <<std::endl;
        return 0.0f;
    }
}
