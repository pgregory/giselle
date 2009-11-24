#include "avartableview.h"
#include "avarlistmodel.h"
#include "luaerror.h"

#include <QMenu>
#include <QContextMenuEvent>
#include <QModelIndex>
#include <QMessageBox>

extern "C" {
#include "lua.h"
}

AvarTableView::AvarTableView(QWidget* parent) : QTableView(parent)
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
        {
            const AvarListItem& av = static_cast<AvarListModel*>(model())->avarListItem(index);
            int avarref = av.avarRef();
            int keyref = av.getKeyframeRef(index.column());
            try
            {
                struct C
                {
                    int avarref;
                    int keyref;
                    static int call(lua_State* L)
                    {
                        C* p = static_cast<C*>(lua_touserdata(L, -1));
                        lua_rawgeti(L, LUA_REGISTRYINDEX, p->avarref);  // Avar
                        lua_getfield(L, -1, "removeKeyframe");          // Avar - removeKeyframe
                        lua_pushvalue(L, -2);        // self            // Avar - removeKeyframe - Avar
                        lua_rawgeti(L, LUA_REGISTRYINDEX, p->keyref);   // Avar - removeKeyframe - Avar - key
                        lua_call(L, 2, 0);                              // Avar
                        lua_pop(L, 1);                                  // --
                        return 0;
                    }
                } p = { avarref, keyref };
                int res = lua_cpcall(m_L, C::call, &p);
                if(res != 0)
                    throw(LuaError(m_L));
            }
            catch(std::exception& e)
            {
                QMessageBox msgBox;
                msgBox.setText(e.what());
                msgBox.exec();
            }
            static_cast<AvarListModel*>(model())->refresh();
        }
    }
}
