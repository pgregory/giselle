#include "avartableview.h"

#include <QMenu>
#include <QContextMenuEvent>
#include <QModelIndex>

AvarTableView::AvarTableView(QWidget* parent) : QTableView(parent)
{
}


void AvarTableView::contextMenuEvent(QContextMenuEvent* e)
{
    QMenu *menu = new QMenu(this);
    QModelIndex index = indexAt(e->pos());
    if(index.isValid())
        menu->addAction(QString("Row %1 - Col %2 was clicked on").arg(index.row()).arg(index.column()));
    else
        menu->addAction("No item was selected");
    menu->exec(QCursor::pos());
}
