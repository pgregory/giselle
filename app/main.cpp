#include <QtGui/QApplication>
#include "mainwindow.h"
#include "glwindow.h"
#include <QSharedPointer>
#include <QDir>
#include <QFile>

#include <iostream>
#include <stdexcept>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lstate.h"
}

#define _QUOTEME(t)    #t
#define QUOTEME(t) _QUOTEME(t)

class LuaError : public std::exception
{
private:
    lua_State * m_L;
    // resource for error object on Lua stack (is to be popped
    // when no longer used)
    QSharedPointer<lua_State> m_lua_resource;
    LuaError & operator=(const LuaError & other); // prevent
public:
    // Construct using top-most element on Lua stack as error.
    LuaError(lua_State * L);
    LuaError(const LuaError & other);
    ~LuaError() throw();
    virtual const char * what() const throw();
};

static void LuaError_lua_resource_delete(lua_State * L)
{
    lua_pop(L, 1);
}

LuaError::LuaError(lua_State * L)
    : m_L(L), m_lua_resource(L, LuaError_lua_resource_delete)
{

}

LuaError::LuaError(const LuaError & other)
    : m_L(other.m_L), m_lua_resource(other.m_lua_resource)
{

}
const char *LuaError::what() const throw()
{
    const char * s = lua_tostring(m_L, -1);
    if (s == NULL) s = "unrecognized Lua error";
    return s;
}
LuaError::~LuaError() throw()
{
}


static int pmain (lua_State *L)
{
    //struct Smain *s = (struct Smain *)lua_touserdata(L, 1);
    const char* current;
    luaL_Buffer b;

    try
    {
        lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
        luaL_openlibs(L);  /* open libraries */
        lua_gc(L, LUA_GCRESTART, 0);

        // Setup package.cpath to find our extensions.
        lua_getglobal(L, "package");
        //lua_getfield(L, -1, "cpath");
        //current = lua_tostring(L, -1);
        //lua_pop(L, 1);
        //luaL_buffinit(L, &b);
        //luaL_addstring(&b, INSTALL_BIN_PATH);
    #ifdef _WIN32
        //luaL_addstring(&b, "/?.dll;");
    #else
        //luaL_addstring(&b, "/lib?.so;");
        //luaL_addstring(&b, INSTALL_BIN_PATH);
        //luaL_addstring(&b, "/?.so;");
    #endif
        //luaL_addstring(&b, current);
        //luaL_pushresult(&b);
        //lua_setfield(L, -2, "cpath");

        lua_getfield(L, -1, "path");
        current = lua_tostring(L, -1);
        lua_pop(L, 1);
        luaL_buffinit(L, &b);
#ifdef  _WIN32
        luaL_addstring(&b, "../../?/init.lua;../../?.lua;");
#else
        luaL_addstring(&b, "../?/init.lua;../?.lua;");
#endif
        luaL_addstring(&b, current);
        luaL_pushresult(&b);
        lua_setfield(L, -2, "path");
        lua_pop(L, 1);

        // Make sure that the TA libraries are all loaded at init time.
        int status = luaL_dostring(L, "require('TA')");
        if(status != 0)
            throw(LuaError(L));

        // Load a simple test lua file for development purposes.
        QDir dir(QUOTEME(ROOT_DIR));
        QString testLua = dir.filePath("test2.lua");
        status = luaL_dofile(L, testLua.toAscii());
        if(status != 0)
            throw(LuaError(L));
    }

    catch(std::exception & e)
    {
        std::cerr << e.what() <<std::endl;
    }

    return 0;
}



int main(int argc, char *argv[])
{
    lua_State *L = lua_open();  /* create state */
    if (L == NULL)
    {
        printf("cannot create state: not enough memory");
        return EXIT_FAILURE;
    }
    pmain(L);

    QApplication a(argc, argv);
    MainWindow w(L);
    w.show();

    GLWindow gl;
    gl.show();

    int result = a.exec();

    lua_close(L);

    return result;
}
