#include "avarlistmodel.h"

AvarListModel::AvarListModel(const QList<AvarListItem>& avars, QObject* parent) :
            QAbstractTableModel(parent), avarList(avars)
{
    // Work out how many columns by looking at all the avars.
    float max = 0.0f, min = 0.0f;
    AvarListItem av;
    foreach(av, avarList)
    {
        QPair<float, float> kf;
        foreach(kf, av.keyframes())
        {
            max = qMax(max, kf.first);
            min = qMin(min, kf.first);
        }
    }
    _startFrame = qRound(min);
    _endFrame = qRound(max);
    _maxColumns = (_endFrame - _startFrame) + 1;
}

int AvarListModel::rowCount(const QModelIndex& parent) const
{
    return avarList.count();
}

int AvarListModel::columnCount(const QModelIndex& parent) const
{
    return _maxColumns;
}


QVariant AvarListModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row() >= avarList.size())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        const AvarListItem& av = avarList.at(index.row());
        QList<QPair<float, float> >::const_iterator kf = av.keyframes().begin(), end = av.keyframes().end();
        while(kf->first < index.column() && kf != end)
            ++kf;
        if(kf != end && kf->first <= index.column())
            return kf->second;
        else
            return QVariant();
    }
    else
        return QVariant();
}

QVariant AvarListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal)
    {
        if(avarList.size() > 0)
            return _startFrame + section;
        else
            return QVariant();
    }
    else
    {
        if(section >= avarList.size())
            return QVariant();
        else
            return avarList.at(section).name();
    }
}


Qt::ItemFlags AvarListModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;

    if(index.row() >= avarList.size())
        return Qt::ItemIsEnabled;

    const AvarListItem& av = avarList.at(index.row());
    QList<QPair<float, float> >::const_iterator kf = av.keyframes().begin(), end = av.keyframes().end();
    while(kf->first < index.column() && kf != end)
        ++kf;
    if(kf != end && kf->first <= index.column())
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    else
        return Qt::ItemIsEnabled;
}


bool AvarListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

    if(index.row() >= avarList.size())
        return false;

    AvarListItem& av = avarList[index.row()];
    QList<QPair<float, float> >::iterator kf = av.keyframes().begin(), end = av.keyframes().end();
    while(kf->first < index.column() && kf != end)
        ++kf;
    if(kf != end && kf->first <= index.column())
    {
        kf->second = value.toDouble();
        emit dataChanged(index, index);
        return true;
    }
    else
        return false;
}
