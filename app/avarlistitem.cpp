#include "avarlistitem.h"

#include <QVariant>

AvarListItem::AvarListItem(const QString& avarName, const QList<QPair<float, float> >& keys) :
        _name(avarName), _keyframes(keys)
{
}


AvarListItem::~AvarListItem()
{
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

