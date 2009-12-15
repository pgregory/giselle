#ifndef AVARLISTMODEL_H
#define AVARLISTMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QList>

extern "C" {
#include "lua.h"
}

#include "avarlistitem.h"

class AvarListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    AvarListModel(lua_State* L, const QList<int>& avars, int startFrame, int endFrame, QObject* parent = 0);
    virtual ~AvarListModel() {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    const AvarListItem& avarListItem(QModelIndex &index) const;
    int keyFrameRef(QModelIndex& index) const;
    void refresh()
    {
        populateModel();
    }
    void clearFrame(QModelIndex& index);

public slots:
    void startFrameChanged(int);
    void endFrameChanged(int);
    void timeChanged(int);

private:
    void populateModel();

    QList<AvarListItem> avarList;
    QList<int>  m_avarRefs;
    int _maxColumns;
    lua_State* m_L;
    int _startFrame;
    int _endFrame;
    int m_currentTime;
};

#endif // AVARLISTMODEL_H
