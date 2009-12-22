#ifndef AVARTABLEVIEW_H
#define AVARTABLEVIEW_H

#include <QTableView>

extern "C" {
#include "lua.h"
}

class AvarTableView : public QTableView
{
   Q_OBJECT

public:
    AvarTableView(QWidget* parent = 0);
    virtual ~AvarTableView() {}

    void contextMenuEvent(QContextMenuEvent* e);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

public slots:
    void timeChanged(int);
    void lock(int);

private:
    bool        m_dragging;
    QModelIndex m_draggingIndex;
    double      m_draggingStartValue;
    QPoint      m_draggingStartPoint;
    double      m_scaleFactor;
    int         m_locked;
};

#endif // AVARTABLEVIEW_H
