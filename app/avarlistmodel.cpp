#include "avarlistmodel.h"

int AvarListModel::rowCount(const QModelIndex& parent) const
{
    return avarList.count();
}

int AvarListModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}


QVariant AvarListModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row() >= avarList.size() || index.column() > 0)
        return QVariant();

    if(role == Qt::DisplayRole)
        return avarList.at(index.row());
    else
        return QVariant();
}

QVariant AvarListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal)
    {
        if(section == 0)
            return QString("Type");
        else
            return QVariant();
    }
    else
    {
        if(section >= avarList.size())
            return QVariant();
        else
            return avarList.at(section);
    }
}
