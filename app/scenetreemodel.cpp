#include "scenetreemodel.h"
#include <iostream>

#include <QtCore/QStringList>

#include "scenetreeitem.h"

extern "C" {
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
}


static void processSceneTree(lua_State* L, SceneTreeItem* parent)
{
    lua_getfield(L, -1, "children");                    // children
    lua_pushnil(L); /* the first key */                 // children - nil
    while(lua_next(L, -2) != 0)                         // children - key - value
    {
        /* 'key' at -2, 'value' at -1 */
        lua_getfield(L, -1, "name");                    // children - key - value - name
        const char* name = lua_tostring(L, -1);
        lua_pop(L, 1);                                  // children - key - value
        lua_pushvalue(L, -1);                           // children - key - value - value
        int nodeRef = luaL_ref(L, LUA_REGISTRYINDEX);   // children - key - value
        QList<QVariant> modelData;
        modelData << name;
        SceneTreeItem* modelItem = new SceneTreeItem(L, modelData, MODEL, nodeRef, parent);
        parent->appendChild(modelItem);
        processSceneTree(L, modelItem);
        lua_pop(L, 1);                                  // children - key
    }
    lua_pop(L, 1);                                      // --
}

SceneTreeModel::SceneTreeModel(lua_State* L, QObject* parent) :
        QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Scene";
    rootItem = new SceneTreeItem(L, rootData, ROOT);

    QList<QVariant> cameraNodeData;
    cameraNodeData << "Cameras";
    SceneTreeItem* camerasItem = new SceneTreeItem(L, cameraNodeData, CAMERAS, LUA_NOREF, rootItem);
    rootItem->appendChild(camerasItem);

    // Now fill in the cameras from the Lua state.
    lua_getglobal(L, "Cameras");                         // Cameras
    lua_pushnil(L); /* the first key */                  // Cameras - nil
    while(lua_next(L, -2) != 0)                          // Cameras - key - value
    {
        /* 'key' at -2, 'value' at -1 */
        lua_getfield(L, -1, "name");                     // Cameras - key - value - name
        const char* name = lua_tostring(L, -1);
        lua_pop(L, 1);                                   // Cameras - key - value
        int nodeRef = luaL_ref(L, LUA_REGISTRYINDEX);    // Cameras - key
        QList<QVariant> cameraData;
        cameraData << name;
        camerasItem->appendChild(new SceneTreeItem(L, cameraData, CAMERA, nodeRef, camerasItem));
    }
    lua_pop(L, 1);                              // --

    QList<QVariant> lightNodeData;
    lightNodeData << "Lights";
    SceneTreeItem* lightsItem = new SceneTreeItem(L, lightNodeData, LIGHTS, LUA_NOREF, rootItem);
    rootItem->appendChild(lightsItem);

    // Now fill in the cameras from the Lua state.
    lua_getglobal(L, "Lightsources");                    // Lightsources
    lua_pushnil(L); /* the first key */                  // Lightsources - nil
    while(lua_next(L, -2) != 0)                          // Lightsources - key - value
    {
        /* 'key' at -2, 'value' at -1 */
        lua_getfield(L, -1, "name");                     // Lightsources - key - value - name
        const char* name = lua_tostring(L, -1);
        lua_pop(L, 1);                                   // Lightsources - key - value
        int nodeRef = luaL_ref(L, LUA_REGISTRYINDEX);    // Lightsources - key
        QList<QVariant> lightData;
        lightData << name;
        lightsItem->appendChild(new SceneTreeItem(L, lightData, LIGHT, nodeRef, lightsItem));
    }
    lua_pop(L, 1);                              // --

    // Add a world node.
    QList<QVariant> worldData;
    worldData << "World";
    lua_getglobal(L, "World");
    int nodeRef = luaL_ref(L, LUA_REGISTRYINDEX);
    SceneTreeItem* worldItem = new SceneTreeItem(L, worldData, WORLD, nodeRef, rootItem);
    rootItem->appendChild(worldItem);

    // Now fill in the models from the Lua state.
    lua_getglobal(L, "World");                          // World
    processSceneTree(L, worldItem);
    lua_pop(L, 1);                                      // --
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

