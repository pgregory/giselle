#include "luaeditor.h"
#include "luahighlighter.h"
#include "datamanager.h"


LuaEditor::LuaEditor() : m_nodeRef(LUA_NOREF)
{
    m_highlighter = new LuaHighlighter(document());
    // Default tab width
    setTabStopWidth(40);
}

LuaEditor::~LuaEditor()
{
    DataManager::instance().releaseRef(m_nodeRef);
}


void LuaEditor::setNodeRef(int ref)
{
    DataManager::instance().releaseRef(m_nodeRef);
    // Duplicate the reference for our ownership.
    m_nodeRef = DataManager::instance().cloneRef(ref);
}
