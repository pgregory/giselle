#ifndef AVARLISTITEM_H
#define AVARLISTITEM_H

#include <QPair>
#include <QList>
#include <QVector>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

class AvarListItem
{
public:
    AvarListItem(lua_State* L, int avarRef, const QString& name, const QVector<int> &keyframes);
    AvarListItem(const AvarListItem& from);
    ~AvarListItem();

    int columnCount() const;
    QVariant data(int column) const;
    const QString& name() const;
    QString& name();
    void setKeyframe(int index, float value);
    QVariant getKeyframeValue(int index) const;
    int getKeyframeRef(int index) const;
    int avarRef() const
    {
        return m_avarRef;
    }

private:
    QString _name;
    int     m_avarRef;
    lua_State* m_L;
    QVector<int> _keyframes;
 };


#endif // AVARLISTITEM_H
