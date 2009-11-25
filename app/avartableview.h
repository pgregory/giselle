#ifndef AVARTABLEVIEW_H
#define AVARTABLEVIEW_H

#include <QTableView>

extern "C" {
#include "lua.h"
}

class AvarTableView : public QTableView
{
public:
    AvarTableView(QWidget* parent = 0);

    void initialiseLua(lua_State* L)
    {
        m_L = L;
    }

    void contextMenuEvent(QContextMenuEvent* e);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

private:
    lua_State* m_L;
    bool        m_dragging;
    QModelIndex m_draggingIndex;
    double      m_draggingStartValue;
    QPoint      m_draggingStartPoint;
    double      m_scaleFactor;
};

#endif // AVARTABLEVIEW_H
