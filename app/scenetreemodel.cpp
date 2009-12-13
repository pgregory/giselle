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
    m_rootItem = new SceneTreeItem(L, rootData, ROOT);

    QList<QVariant> cameraNodeData;
    cameraNodeData << "Cameras";
    m_camerasItem = new SceneTreeItem(L, cameraNodeData, CAMERAS, LUA_NOREF, m_rootItem);
    m_rootItem->appendChild(m_camerasItem);

    QList<QVariant> lightNodeData;
    lightNodeData << "Lights";
    m_lightsItem = new SceneTreeItem(L, lightNodeData, LIGHTS, LUA_NOREF, m_rootItem);
    m_rootItem->appendChild(m_lightsItem);

    // Add a world node.
    QList<QVariant> worldData;
    worldData << "World";
    lua_getglobal(L, "World");
    int nodeRef = luaL_ref(L, LUA_REGISTRYINDEX);
    m_worldItem = new SceneTreeItem(L, worldData, WORLD, nodeRef, m_rootItem);
    m_rootItem->appendChild(m_worldItem);

    populateData(L);
}


SceneTreeModel::~SceneTreeModel()
{
    delete m_rootItem;
}


void SceneTreeModel::processSceneTree(lua_State* L, SceneTreeItem* parent, const QModelIndex& wi)
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
        QList<QVariant> modelData;
        modelData << name;
        SceneTreeItem* modelItem = 0;
        int row = 0;
        if(!parent->contains(modelData, row))
        {
            beginInsertRows(wi, parent->childCount(), parent->childCount());
            row = parent->childCount();
            int nodeRef = luaL_ref(L, LUA_REGISTRYINDEX);   // children - key - value
            modelItem = new SceneTreeItem(L, modelData, MODEL, nodeRef, parent);
            parent->appendChild(modelItem);
            endInsertRows();
        }
        else
        {
            lua_pop(L, 1);                              // children - key - value
            modelItem = parent->child(row);
        }
        QModelIndex ci = index(row, 0, wi);
        processSceneTree(L, modelItem, ci);
        lua_pop(L, 1);                                  // children - key
    }
    lua_pop(L, 1);                                      // --
}


void SceneTreeModel::populateData(lua_State* L)
{
    // Now fill in the cameras from the Lua state.
    lua_getglobal(L, "Cameras");                         // Cameras
    lua_pushnil(L); /* the first key */                  // Cameras - nil
    QModelIndex ci = index(0, 0, QModelIndex());
    while(lua_next(L, -2) != 0)                          // Cameras - key - value
    {
        /* 'key' at -2, 'value' at -1 */
        lua_getfield(L, -1, "name");                     // Cameras - key - value - name
        const char* name = lua_tostring(L, -1);
        lua_pop(L, 1);                                   // Cameras - key - value
        int row;
        QList<QVariant> cameraData;
        cameraData << name;
        if(!m_camerasItem->contains(cameraData, row))
        {
            beginInsertRows(ci, m_camerasItem->childCount(), m_camerasItem->childCount());
            int nodeRef = luaL_ref(L, LUA_REGISTRYINDEX);    // Cameras - key
            m_camerasItem->appendChild(new SceneTreeItem(L, cameraData, CAMERA, nodeRef, m_camerasItem));
            endInsertRows();
        }
        else
            lua_pop(L, 1);                              // Cameras - key
    }
    lua_pop(L, 1);                              // --

    // Now fill in the cameras from the Lua state.
    lua_getglobal(L, "Lightsources");                    // Lightsources
    lua_pushnil(L); /* the first key */                  // Lightsources - nil
    QModelIndex li = index(1, 0, QModelIndex());
    while(lua_next(L, -2) != 0)                          // Lightsources - key - value
    {
        /* 'key' at -2, 'value' at -1 */
        lua_getfield(L, -1, "name");                     // Lightsources - key - value - name
        const char* name = lua_tostring(L, -1);
        lua_pop(L, 1);                                   // Lightsources - key - value
        QList<QVariant> lightData;
        lightData << name;
        int row;
        if(!m_lightsItem->contains(lightData, row))
        {
            beginInsertRows(li, m_lightsItem->childCount(), m_lightsItem->childCount());
            int nodeRef = luaL_ref(L, LUA_REGISTRYINDEX);    // Lightsources - key
            m_lightsItem->appendChild(new SceneTreeItem(L, lightData, LIGHT, nodeRef, m_lightsItem));
            endInsertRows();
        }
        else
            lua_pop(L, 1);                              // Lightsources - key
    }
    lua_pop(L, 1);                              // --

    // Now fill in the models from the Lua state.
    lua_getglobal(L, "World");                          // World
    QModelIndex wi = index(2, 0, QModelIndex());
    processSceneTree(L, m_worldItem, wi);
    lua_pop(L, 1);                                      // --
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

