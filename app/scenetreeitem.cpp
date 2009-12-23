#include "scenetreeitem.h"
#include "datamanager.h"

#include <QStringList>


SceneTreeItem::SceneTreeItem(const QList<QVariant> &data, TreeItemType type, int nodeRef, SceneTreeItem *parent)
{
    parentItem = parent;
    itemData = data;
    itemType = type;

    // Duplicate the reference for our ownership.
    m_nodeRef = DataManager::instance().cloneRef(nodeRef);
}

SceneTreeItem::~SceneTreeItem()
{
    DataManager::instance().releaseRef(m_nodeRef);
    qDeleteAll(childItems);
}

void SceneTreeItem::appendChild(SceneTreeItem *item)
{
    childItems.append(item);
}

bool SceneTreeItem::contains(QList<QVariant>& data, int& row)
{
    QListIterator<SceneTreeItem*> i(childItems);
    row = 0;
    while(i.hasNext())
    {
        if(*(i.next()) == data)
            return true;
        ++row;
    }
    return false;
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
