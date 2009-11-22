#include "scenetreeitem.h"

#include <QStringList>


SceneTreeItem::SceneTreeItem(lua_State* L, const QList<QVariant> &data, TreeItemType type, int nodeRef, SceneTreeItem *parent) :
        m_L(L)
{
    parentItem = parent;
    itemData = data;
    itemType = type;
    // Duplicate the reference for our ownership.
    lua_rawgeti(L, LUA_REGISTRYINDEX, nodeRef);
    m_nodeRef = luaL_ref(L, LUA_REGISTRYINDEX);
}

SceneTreeItem::~SceneTreeItem()
{
    if(m_nodeRef != LUA_NOREF)
        luaL_unref(m_L, LUA_REGISTRYINDEX, m_nodeRef);
    qDeleteAll(childItems);
}

void SceneTreeItem::appendChild(SceneTreeItem *item)
{
    childItems.append(item);
}

SceneTreeItem *SceneTreeItem::child(int row)
{
    return childItems.value(row);
}

int SceneTreeItem::childCount() const
{
    return childItems.count();
}

int SceneTreeItem::columnCount() const
{
    return itemData.count();
}

QVariant SceneTreeItem::data(int column) const
{
    return itemData.value(column);
}

SceneTreeItem *SceneTreeItem::parent()
{
    return parentItem;
}

int SceneTreeItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<SceneTreeItem*>(this));

    return 0;
}

TreeItemType SceneTreeItem::type() const
{
    return itemType;
}
