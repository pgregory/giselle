#ifndef SCENETREEMODEL_H
#define SCENETREEMODEL_H

#include <QtCore/QAbstractItemModel>

class SceneTreeItem;

class SceneTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    SceneTreeModel(QObject *parent = 0);
    virtual ~SceneTreeModel();

    void processSceneTree(int parentRef, SceneTreeItem* parent, const QModelIndex& index);
    void populateData();

    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

private:
    SceneTreeItem* m_rootItem;
    SceneTreeItem* m_worldItem;
};

#endif // SCENETREEMODEL_H
