#include "scenetreeitem.h"

#include <QStringList>


SceneTreeItem::SceneTreeItem(const QList<QVariant> &data, SceneTreeItem *parent)
{
    parentItem = parent;
    itemData = data;
}

SceneTreeItem::~SceneTreeItem()
{
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
