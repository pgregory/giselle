#ifndef SCENETREEITEM_H
#define SCENETREEITEM_H

#include <QList>
#include <QVariant>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

enum _TreeItemType
{
    ROOT,
    CAMERAS,
    CAMERA,
    MODEL,
    WORLD,
};

typedef _TreeItemType TreeItemType;

class SceneTreeItem
{
public:
    SceneTreeItem(const QList<QVariant> &data, TreeItemType type, int nodeRef = LUA_NOREF, SceneTreeItem *parent = 0);
    ~SceneTreeItem();

    void appendChild(SceneTreeItem *child);
    bool contains(QList<QVariant>& data, int& row);

    SceneTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    SceneTreeItem *parent();
    TreeItemType type() const;
    int nodeRef() const
    {
        return m_nodeRef;
    }

    bool operator==(const QList<QVariant>& other)
    {
        return itemData == other;
    }

private:
    QList<SceneTreeItem*> childItems;
    QList<QVariant> itemData;
    TreeItemType itemType;
    SceneTreeItem *parentItem;
    int m_nodeRef;
 };


#endif // SCENETREEITEM_H
