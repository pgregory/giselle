#include "luaeditor.h"
#include "luahighlighter.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

LuaEditor::LuaEditor(lua_State* L) : m_nodeRef(LUA_NOREF), m_L(L)
{
    m_highlighter = new LuaHighlighter(document());
    // Default tab width
    setTabStopWidth(40);
}

LuaEditor::~LuaEditor()
{
    luaL_unref(m_L, LUA_REGISTRYINDEX, m_nodeRef);
}


void LuaEditor::setNodeRef(int ref)
{
    luaL_unref(m_L, LUA_REGISTRYINDEX, m_nodeRef);
    // Duplicate the reference for our ownership.
    lua_rawgeti(m_L, LUA_REGISTRYINDEX, ref);
    m_nodeRef = luaL_ref(m_L, LUA_REGISTRYINDEX);
}
