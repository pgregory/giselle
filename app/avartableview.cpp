#include "avartableview.h"
#include "avarlistmodel.h"
#include "luaerror.h"

#include <QMenu>
#include <QContextMenuEvent>
#include <QModelIndex>
#include <QMessageBox>
#include <QCheckBox>

extern "C" {
#include "lua.h"
}

AvarTableView::AvarTableView(QWidget* parent) : QTableView(parent),
        m_dragging(false), m_scaleFactor(0.01), m_locked(false)
{
}

void AvarTableView::contextMenuEvent(QContextMenuEvent* e)
{
    QMenu *menu = new QMenu(this);
    QModelIndex index = indexAt(e->pos());
    if(index.isValid())
    {
        menu->addAction("Delete Keyframe");
        if(menu->exec(QCursor::pos()) != 0)
            static_cast<AvarListModel*>(model())->clearFrame(index);
    }
    delete menu;
}


void AvarTableView::mousePressEvent(QMouseEvent* event)
{
    m_draggingIndex = indexAt(event->pos());
    if(m_draggingIndex.isValid())
    {
        m_dragging = true;
        m_draggingStartPoint = event->pos();
        m_draggingStartValue = model()->data(m_draggingIndex, Qt::DisplayRole).toDouble();
        //grabMouse();
    }
    QTableView::mousePressEvent(event);
}

void AvarTableView::mouseMoveEvent(QMouseEvent* event)
{
    if(m_dragging)
    {
        QPoint p = event->pos();
        double diff = p.y() - m_draggingStartPoint.y();
        diff *= m_scaleFactor;
        model()->setData(m_draggingIndex, m_draggingStartValue + diff, Qt::DisplayRole);
    }
}

void AvarTableView::mouseReleaseEvent(QMouseEvent* event)
{
    if(m_dragging)
    {
        m_dragging = false;
        //releaseMouse();
    }
}


void AvarTableView::timeChanged(int time)
{
    if(m_locked)
        scrollTo(model()->index(0, time), QAbstractItemView::PositionAtCenter);
    repaint();
}

void AvarTableView::lock(int state)
{
    m_locked = state;
}
