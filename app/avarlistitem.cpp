#include "avarlistitem.h"

#include <QVariant>

extern "C" {
#include "lauxlib.h"
}
#include "luaerror.h"
#include "datamanager.h"

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
        DataManager::instance().setKeyframeFromRef(_keyframes[index], value);
    else
        _keyframes[index] = DataManager::instance().addKeyframe(m_avarRef, index, value);
}


QVariant AvarListItem::getKeyframeValue(int index) const
{
    if(_keyframes.size() <= index || _keyframes[index] == LUA_NOREF)
        return QVariant();
    return DataManager::instance().getKeyframe(_keyframes[index]);
}


int AvarListItem::getKeyframeRef(int index) const
{
    return _keyframes[index];
}
