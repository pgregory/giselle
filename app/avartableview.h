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

private:
    lua_State* m_L;
};

#endif // AVARTABLEVIEW_H
