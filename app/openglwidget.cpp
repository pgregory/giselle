#include "openglwidget.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include "luaerror.h"

void OpenGLWidget::initialiseLua(lua_State* L)
{
    m_L = L;
    // Now create a new GLRenderer for our use.
    try
    {
        struct C
        {
            int glrendref;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, -1));
                lua_getglobal(L, "GLRenderer");     // GLRenderer
                lua_getfield(L, -1, "create");      // GLRenderer - create
                lua_pushvalue(L, -2);   // self     // GLRenderer - create - GLRenderer
                lua_pushstring(L, "test");          // GLRenderer - create - GLRenderer - name
                lua_call(L, 2, 1);                  // GLRenderer - newrenderer
                p->glrendref = luaL_ref(L, LUA_REGISTRYINDEX);     // GLRenderer
                lua_pop(L, 1);
                return 0;
            }
        } p = { 0 };
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
        m_glRendererRef = p.glrendref;
    }
    catch(std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
    }
}


void OpenGLWidget::paintGL()
{
    try
    {
        struct C
        {
            int rendererref;
            int time;
            QString cameraName;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, 1));
                lua_rawgeti(L, LUA_REGISTRYINDEX, p->rendererref);
                lua_getfield(L, -1, "renderIt");
                lua_pushvalue(L, -2);
                lua_newtable(L);
                lua_getglobal(L, "World");
                lua_setfield(L, -2, "world");
                if(!p->cameraName.isEmpty())
                {
                    lua_getglobal(L, "Cameras");
                    lua_getfield(L, -1, p->cameraName.toAscii());
                    lua_setfield(L, -3, "camera");
                    lua_pop(L, 1);  // << Cameras
                }
                lua_pushinteger(L, p->time);
                lua_setfield(L, -2, "start");
                lua_pushinteger(L, p->time);
                lua_setfield(L, -2, "stop");
                lua_call(L, 2, 0);
                lua_pop(L, 1);  /* << renderer" */
                return 0;
            }
        } p = { m_glRendererRef, m_time, m_cameraName };
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
    }
    catch(std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        // \todo: Want to output the message to the interface here, but if
        // I do, the render loop forces us into an infinite loop, so for now it
        // just goes to stderr.
        //QMessageBox msgBox;
        //msgBox.setText(e.what());
        //msgBox.exec();
    }
}

void OpenGLWidget::timeChanged(int time)
{
    m_time = time;
    updateGL();
}

void OpenGLWidget::cameraChanged(QString cameraName)
{
    m_cameraName = cameraName;
    updateGL();
}
