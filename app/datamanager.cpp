#include <QMessageBox>

#include "datamanager.h"

#include "luaerror.h"
#include <assert.h>


int DataManager::cloneRef(int ref)
{
    lua_rawgeti(m_L, LUA_REGISTRYINDEX, ref);
    return luaL_ref(m_L, LUA_REGISTRYINDEX);
}

void DataManager::releaseRef(int ref)
{
    luaL_unref(m_L, LUA_REGISTRYINDEX, ref);
}


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

float DataManager::getKeyframeFromRef(int keyframeRef)
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


QString DataManager::getAvarNameFromRef(int avarRef)
{
    struct C
    {
        int avarRef;
        QString name;
        static int call(lua_State* L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, 1));
            lua_rawgeti(L, LUA_REGISTRYINDEX, p->avarRef);
            lua_getfield(L, -1, "name");
            p->name = lua_tostring(L, -1);
            lua_pop(L, 2);  /* << name << ref*/
            return 0;
        }
    } p = { avarRef, "" };
    try
    {
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
        return p.name;
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() <<std::endl;
    }
    return "";
}

void DataManager::getKeyframesFromRef(int avarRef, QList<Keyframe>& kfs)
{
    struct C
    {
        int avarRef;
        QList<Keyframe>& keyframes;
        QString name;
        static int call(lua_State* L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, 1));
            lua_rawgeti(L, LUA_REGISTRYINDEX, p->avarRef);
            lua_getfield(L, -1, "name");
            p->name = lua_tostring(L, -1);
            lua_pop(L, 1);  /* << name */
            lua_getfield(L, -1, "keyframes");
            lua_pushnil(L); /* the first key */
            while(lua_next(L, -2) != 0)
            {
                lua_getfield(L, -1, "frame");
                int frame = lua_tointeger(L, -1);
                lua_getfield(L, -2, "value");
                float value = lua_tonumber(L, -1);
                lua_pop(L, 2); // << value << frame
                int ref = luaL_ref(L, LUA_REGISTRYINDEX);
                Keyframe kf = { ref, frame, value };
                p->keyframes << kf;
            }
            lua_pop(L, 2); /* << keyframes << ref */
            return 0;
        }
    } p = { avarRef, kfs, "" };
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

void DataManager::deleteKeyframeFromRef(int avarRef, int keyframeRef)
{
    try
    {
        struct C
        {
            int avarref;
            int keyref;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, -1));
                lua_rawgeti(L, LUA_REGISTRYINDEX, p->avarref);  // Avar
                lua_getfield(L, -1, "removeKeyframe");          // Avar - removeKeyframe
                lua_pushvalue(L, -2);        // self            // Avar - removeKeyframe - Avar
                lua_rawgeti(L, LUA_REGISTRYINDEX, p->keyref);   // Avar - removeKeyframe - Avar - key
                lua_call(L, 2, 0);                              // Avar
                lua_pop(L, 1);                                  // --
                return 0;
            }
        } p = { avarRef, keyframeRef };
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
    }
    catch(std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
    }
}
