#ifndef SCENETREEITEM_H
#define SCENETREEITEM_H

#include <QList>
#include <QVariant>

enum _TreeItemType
{
    ROOT,
    MODELS,
    CAMERAS,
    MODEL,
    CAMERA,
};

typedef _TreeItemType TreeItemType;

class SceneTreeItem
{
public:
    SceneTreeItem(const QList<QVariant> &data, TreeItemType type, SceneTreeItem *parent = 0);
    ~SceneTreeItem();

    void appendChild(SceneTreeItem *child);

    SceneTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    SceneTreeItem *parent();
    TreeItemType type() const;

private:
    QList<SceneTreeItem*> childItems;
    QList<QVariant> itemData;
    TreeItemType itemType;
    SceneTreeItem *parentItem;
 };


#endif // SCENETREEITEM_H
