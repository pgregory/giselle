#ifndef LUAEDITOR_H
#define LUAEDITOR_H

#include <QTextEdit>

class LuaHighlighter;

class LuaEditor : public QTextEdit
{
public:
    LuaEditor();
    virtual ~LuaEditor();

    int nodeRef() const
    {
        return m_nodeRef;
    }
    void setNodeRef(int ref);

private:
    int m_nodeRef;
    LuaHighlighter* m_highlighter;
};

#endif // LUAEDITOR_H
