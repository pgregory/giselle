#include "avarlistitem.h"

#include <QVariant>
#include <QMessageBox>

extern "C" {
#include "lauxlib.h"
}
#include "luaerror.h"
#include "datamanager.h"

AvarListItem::AvarListItem(int avarRef, const QString& avarName, const QVector<int>& keys) :
        _name(avarName), _keyframes(keys)
{
    // Duplicate the reference for our ownership.
    m_avarRef = DataManager::instance().cloneRef(avarRef);
}

AvarListItem::AvarListItem(const AvarListItem& from) :
        _name(from._name), _keyframes(from._keyframes)
{
    // Duplicate the reference for our ownership.
    m_avarRef = DataManager::instance().cloneRef(from.m_avarRef);
}

AvarListItem::~AvarListItem()
{
    DataManager::instance().releaseRef(m_avarRef);
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
    try
    {
        if(_keyframes[index] != LUA_NOREF)
            DataManager::instance().setKeyframeFromRef(_keyframes[index], value);
        else
            _keyframes[index] = DataManager::instance().addKeyframe(m_avarRef, index, value);
    }
    catch(LuaError& e)
    {
        QMessageBox box;
        box.setText(e.what());
        box.exec();
    }
}


QVariant AvarListItem::getKeyframeValue(int index) const
{
    if(_keyframes.size() <= index || _keyframes[index] == LUA_NOREF)
        return QVariant();
    try
    {
        return DataManager::instance().getKeyframeFromRef(_keyframes[index]);
    }
    catch(LuaError& e)
    {
        QMessageBox box;
        box.setText(e.what());
        box.exec();
        return QVariant();
    }
}


int AvarListItem::getKeyframeRef(int index) const
{
    return _keyframes[index];
}
