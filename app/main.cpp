#include <QtGui/QApplication>
#include "mainwindow.h"
#include "glwindow.h"
#include <QSharedPointer>
#include <QDir>
#include <QFile>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lstate.h"
#include "luagl.h"
#include "luaglu.h"
}

#include "luaerror.h"

#define _QUOTEME(t)    #t
#define QUOTEME(t) _QUOTEME(t)


static int pmain (lua_State *L)
{
    //struct Smain *s = (struct Smain *)lua_touserdata(L, 1);
    const char* current;
    luaL_Buffer b;

    try
    {
        lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
        luaL_openlibs(L);  /* open libraries */
        luaopen_luagl(L);
        luaopen_luaglu(L);
        lua_gc(L, LUA_GCRESTART, 0);

        // Setup package.cpath to find our extensions.
        lua_getglobal(L, "package");
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

    int result = a.exec();

    lua_close(L);

    return result;
}
