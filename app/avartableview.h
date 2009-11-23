#ifndef AVARTABLEVIEW_H
#define AVARTABLEVIEW_H

#include <QTableView>

class AvarTableView : public QTableView
{
public:
    AvarTableView(QWidget* parent = 0);

    void contextMenuEvent(QContextMenuEvent* e);
};

#endif // AVARTABLEVIEW_H
