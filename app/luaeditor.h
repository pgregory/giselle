#ifndef LUAEDITOR_H
#define LUAEDITOR_H

#include <QTextEdit>

struct lua_State;
class LuaHighlighter;

class LuaEditor : public QTextEdit
{
public:
    LuaEditor(lua_State* L);
    ~LuaEditor();

    int nodeRef() const
    {
        return m_nodeRef;
    }
    void setNodeRef(int ref);

private:
    int m_nodeRef;
    lua_State* m_L;
    LuaHighlighter* m_highlighter;
};

#endif // LUAEDITOR_H
