#include "scenetreemodel.h"
#include "datamanager.h"
#include <iostream>

#include <QStringList>
#include <QMessageBox>

#include "scenetreeitem.h"


SceneTreeModel::SceneTreeModel(QObject* parent) :
        QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Scene";
    m_rootItem = new SceneTreeItem(rootData, ROOT);

    try
    {
        // Add a world node.
        QList<QVariant> worldData;
        worldData << "World";
        int nodeRef = DataManager::instance().getWorldRef();
        m_worldItem = new SceneTreeItem(worldData, WORLD, nodeRef, m_rootItem);
        m_rootItem->appendChild(m_worldItem);

        populateData();
    }
    catch(LuaError& e)
    {
        QMessageBox box;
        box.setText(e.what());
        box.exec();
    }
}


SceneTreeModel::~SceneTreeModel()
{
    delete m_rootItem;
}


void SceneTreeModel::processSceneTree(int parentRef, SceneTreeItem* parent, const QModelIndex& wi)
{
    try
    {
        QList<int> children;
        DataManager::instance().getNodeChildrendRefsFromRef(parentRef, children);
        int child;
        foreach(child, children)
        {
            QString name = DataManager::instance().nodeNameFromRef(child);
            QList<QVariant> modelData;
            modelData << name;
            SceneTreeItem* modelItem = 0;
            int row;
            if(!parent->contains(modelData, row))
            {
                beginInsertRows(wi, parent->childCount(), parent->childCount());
                row = parent->childCount();
                modelItem = new SceneTreeItem(modelData, MODEL, child, parent);
                parent->appendChild(modelItem);
                endInsertRows();
            }
            else
                modelItem = parent->child(row);
            QModelIndex ci = index(row, 0, wi);
            processSceneTree(child, modelItem, ci);
        }
    }
    catch(LuaError& e)
    {
        QMessageBox box;
        box.setText(e.what());
        box.exec();
    }
}


void SceneTreeModel::populateData()
{
    // Now fill in the models from the Lua state.
    try
    {
        int worldRef = DataManager::instance().getWorldRef();
        QModelIndex wi = index(1 , 0, QModelIndex());
        processSceneTree(worldRef, m_worldItem, wi);
    }
    catch(LuaError& e)
    {
        QMessageBox box;
        box.setText(e.what());
        box.exec();
    }
}


int SceneTreeModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return static_cast<SceneTreeItem*>(parent.internalPointer())->columnCount();
    else
        return m_rootItem->columnCount();
}

 QVariant SceneTreeModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     if (role != Qt::DisplayRole)
         return QVariant();

     SceneTreeItem *item = static_cast<SceneTreeItem*>(index.internalPointer());

     return item->data(index.column());
 }

 Qt::ItemFlags SceneTreeModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid())
         return 0;

     return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
 }

 QVariant SceneTreeModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
 {
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
         return m_rootItem->data(section);

     return QVariant();
 }

 QModelIndex SceneTreeModel::index(int row, int column, const QModelIndex &parent)
             const
 {
     if (!hasIndex(row, column, parent))
         return QModelIndex();

     SceneTreeItem *parentItem;

     if (!parent.isValid())
         parentItem = m_rootItem;
     else
         parentItem = static_cast<SceneTreeItem*>(parent.internalPointer());

     SceneTreeItem *childItem = parentItem->child(row);
     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
 }

 QModelIndex SceneTreeModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     SceneTreeItem *childItem = static_cast<SceneTreeItem*>(index.internalPointer());
     SceneTreeItem *parentItem = childItem->parent();

     if (parentItem == m_rootItem)
         return QModelIndex();

     return createIndex(parentItem->row(), 0, parentItem);
 }

 int SceneTreeModel::rowCount(const QModelIndex &parent) const
 {
     SceneTreeItem *parentItem;
     if (parent.column() > 0)
         return 0;

     if (!parent.isValid())
         parentItem = m_rootItem;
     else
         parentItem = static_cast<SceneTreeItem*>(parent.internalPointer());

     return parentItem->childCount();
 }

