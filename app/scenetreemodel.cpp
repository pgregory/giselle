#include "scenetreemodel.h"
#include <iostream>

#include <QtCore/QStringList>

#include "scenetreeitem.h"

extern "C" {
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
}


SceneTreeModel::SceneTreeModel(lua_State* L, QObject* parent) :
        QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Scene";
    rootItem = new SceneTreeItem(rootData, ROOT);
    QList<QVariant> modelNodeData;
    modelNodeData << "Models";
    SceneTreeItem* modelsItem = new SceneTreeItem(modelNodeData, MODELS, rootItem);
    rootItem->appendChild(modelsItem);

    // Now fill in the models from the Lua state.
    lua_getglobal(L, "Models");
    lua_pushnil(L); /* the first key */
    while(lua_next(L, -2) != 0)
    {
        /* 'key' at -2, 'value' at -1 */
        lua_getfield(L, -1, "name");
        const char* name = lua_tostring(L, -1);
        QList<QVariant> modelData;
        modelData << name;
        modelsItem->appendChild(new SceneTreeItem(modelData, MODEL, modelsItem));
        lua_pop(L, 1);
        lua_pop(L, 1);  /* pop value, leave key for next iteration */
    }
    lua_pop(L, 1);

    QList<QVariant> cameraNodeData;
    cameraNodeData << "Cameras";
    SceneTreeItem* camerasItem = new SceneTreeItem(cameraNodeData, CAMERAS, rootItem);
    rootItem->appendChild(camerasItem);

    // Now fill in the cameras from the Lua state.
    lua_getglobal(L, "Cameras");
    lua_pushnil(L); /* the first key */
    while(lua_next(L, -2) != 0)
    {
        /* 'key' at -2, 'value' at -1 */
        lua_getfield(L, -1, "name");
        const char* name = lua_tostring(L, -1);
        QList<QVariant> cameraData;
        cameraData << name;
        camerasItem->appendChild(new SceneTreeItem(cameraData, CAMERA, camerasItem));
        lua_pop(L, 1);
        lua_pop(L, 1);  /* pop value, leave key for next iteration */
    }
    lua_pop(L, 2);
}

SceneTreeModel::~SceneTreeModel()
{
    delete rootItem;
}

int SceneTreeModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return static_cast<SceneTreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
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
         return rootItem->data(section);

     return QVariant();
 }

 QModelIndex SceneTreeModel::index(int row, int column, const QModelIndex &parent)
             const
 {
     if (!hasIndex(row, column, parent))
         return QModelIndex();

     SceneTreeItem *parentItem;

     if (!parent.isValid())
         parentItem = rootItem;
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

     if (parentItem == rootItem)
         return QModelIndex();

     return createIndex(parentItem->row(), 0, parentItem);
 }

 int SceneTreeModel::rowCount(const QModelIndex &parent) const
 {
     SceneTreeItem *parentItem;
     if (parent.column() > 0)
         return 0;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<SceneTreeItem*>(parent.internalPointer());

     return parentItem->childCount();
 }

