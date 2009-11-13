#ifndef AVARLISTMODEL_H
#define AVARLISTMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QList>

#include "avarlistitem.h"

class AvarListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    AvarListModel(const QList<AvarListItem>& avars, QObject* parent = 0);

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

private:
    QList<AvarListItem> avarList;
    int _startFrame;
    int _endFrame;
    int _maxColumns;
};

#endif // AVARLISTMODEL_H
