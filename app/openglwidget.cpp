#include "openglwidget.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include "luaerror.h"

void OpenGLWidget::paintGL()
{
    try
    {
        struct C
        {
            int time;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, 1));
                lua_getglobal(L, "theGLRenderer");
                lua_getfield(L, -1, "renderIt");
                lua_pushvalue(L, -2);
                lua_newtable(L);
                lua_getglobal(L, "theWorld");
                lua_setfield(L, -2, "world");
                lua_getglobal(L, "theCamera");
                lua_setfield(L, -2, "camera");
                lua_pushinteger(L, p->time);
                lua_setfield(L, -2, "start");
                lua_pushinteger(L, p->time);
                lua_setfield(L, -2, "stop");
                lua_pcall(L, 2, 0, 0);
                lua_pop(L, 1);  /* << theGLRenderer" */
                return 0;
            }
        } p = { m_time };
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));

    }

    catch(std::exception & e)
    {
        std::cerr << e.what() <<std::endl;
    }
}

void OpenGLWidget::timeChanged(int time)
{
    m_time = time;
    updateGL();
}
