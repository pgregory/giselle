#include "avarlistitem.h"

#include <QVariant>

extern "C" {
#include "lauxlib.h"
}
#include "luaerror.h"

AvarListItem::AvarListItem(lua_State* L, int avarRef, const QString& avarName, const QVector<int>& keys) :
        _name(avarName), m_L(L), _keyframes(keys)
{
    // Duplicate the reference for our ownership.
    lua_rawgeti(L, LUA_REGISTRYINDEX, avarRef);
    m_avarRef = luaL_ref(L, LUA_REGISTRYINDEX);
}

AvarListItem::AvarListItem(const AvarListItem& from) :
        _name(from._name), m_L(from.m_L), _keyframes(from._keyframes)
{
    // Duplicate the reference for our ownership.
    lua_rawgeti(m_L, LUA_REGISTRYINDEX, from.m_avarRef);
    m_avarRef = luaL_ref(m_L, LUA_REGISTRYINDEX);
}

AvarListItem::~AvarListItem()
{
    luaL_unref(m_L, LUA_REGISTRYINDEX, m_avarRef);
}

int AvarListItem::columnCount() const
{
    return _keyframes.size();
}


const QString& AvarListItem::name() const
{
    return _name;
}

QString& AvarListItem::name()
{
    return _name;
}


void AvarListItem::setKeyframe(int index, float value)
{
    if(_keyframes[index] != LUA_NOREF)
    {
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
        } p = { _keyframes[index], value };
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
    else
    {
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
        } p = { m_avarRef, index, value, 0 };
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
        _keyframes[index] = p.keyRef;
    }
}


QVariant AvarListItem::getKeyframeValue(int index) const
{
    if(_keyframes.size() <= index || _keyframes[index] == LUA_NOREF)
        return QVariant();

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
    } p = { _keyframes[index], 0.0f };
    try
    {
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
        return p.value;
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() <<std::endl;
        return QVariant();
    }
}
