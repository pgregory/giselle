#include "avarlistitem.h"

#include <QVariant>

extern "C" {
#include "lauxlib.h"
}
#include "luaerror.h"

AvarListItem::AvarListItem(lua_State* L, int avarRef, const QString& avarName, const QList<QPair<float, float> >& keys) :
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

QVariant AvarListItem::data(int column) const
{
    if(column < _keyframes.size())
        return _keyframes[column].second;
    else
        return QVariant();
}

const QString& AvarListItem::name() const
{
    return _name;
}

QString& AvarListItem::name()
{
    return _name;
}

QList<QPair<float, float> >& AvarListItem::keyframes()
{
    return _keyframes;
}

const QList<QPair<float, float> >& AvarListItem::keyframes() const
{
    return _keyframes;
}

void AvarListItem::setKeyframe(float time, float value)
{
    QList<QPair<float, float> >::iterator kf = keyframes().begin(), end = keyframes().end();
    while(kf->first < time && kf != end)
        ++kf;

    if(kf != end && kf->first <= time)
    {
        kf->second = value;
        struct C
        {
            int avarRef;
            float time;
            float value;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, 1));
                lua_rawgeti(L, LUA_REGISTRYINDEX, p->avarRef);
                lua_getfield(L, -1, "keyframes");
                lua_pushinteger(L, qRound(p->time)+1);
                lua_newtable(L);
                lua_pushnumber(L, p->time);
                lua_setfield(L, -2, "time");
                lua_pushnumber(L, p->value);
                lua_setfield(L, -2, "value");
                lua_settable(L, -2);
                lua_pop(L, 2); /* << keyframes << ref */
                return 0;
            }
        } p = { m_avarRef, time, value };
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
}
