#ifndef AVARLISTITEM_H
#define AVARLISTITEM_H

#include <QPair>
#include <QList>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

class AvarListItem
{
public:
    AvarListItem(lua_State* L, int avarRef, const QString& name, const QList<QPair<float, float> > &keyframes);
    AvarListItem(const AvarListItem& from);
    ~AvarListItem();

    int columnCount() const;
    QVariant data(int column) const;
    const QString& name() const;
    QString& name();
    QList<QPair<float, float> >& keyframes();
    const QList<QPair<float, float> >& keyframes() const;
    void setKeyframe(float time, float value);

private:
    QString _name;
    int     m_avarRef;
    lua_State* m_L;
    QList<QPair<float, float> > _keyframes;
 };


#endif // AVARLISTITEM_H
