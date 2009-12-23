#include <QMessageBox>

#include "datamanager.h"

#include "luaerror.h"
#include <assert.h>


int DataManager::cloneRef(int ref)
{
    lua_rawgeti(m_L, LUA_REGISTRYINDEX, ref);
    return luaL_ref(m_L, LUA_REGISTRYINDEX);
}

void DataManager::releaseRef(int ref)
{
    luaL_unref(m_L, LUA_REGISTRYINDEX, ref);
}

bool DataManager::refsEqual(int refa, int refb)
{
    try
    {
        struct C
        {
            int indexa, indexb;
            bool equal;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, -1));
                lua_rawgeti(L, LUA_REGISTRYINDEX, p->indexa);
                lua_rawgeti(L, LUA_REGISTRYINDEX, p->indexb);
                p->equal = lua_rawequal(L, -1, -2) != 0;
                lua_pop(L, 2);
                return 0;
            }
        } p = { refa, refb, false };
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));

        return p.equal;
    }
    catch(std::exception& e)
    {
        QMessageBox box;
        box.setText(e.what());
        box.exec();
        return false;
    }
}


void DataManager::runCommand(const QString& command) throw(LuaError)
{
    int res = luaL_dostring(m_L, command.toAscii());
    if(res != 0)
    {
        throw(LuaError(m_L));
    }
}


void DataManager::saveData(const QString& filename) throw(LuaError)
{
    struct C
    {
        QString name;
        static int call(lua_State* L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, -1));
            lua_getglobal(L, "saveAll");        // saveAll
            lua_pushstring(L, p->name.toAscii());// saveAll - name
            lua_call(L, 1, 0);                  // --

            return 0;
        }
    } p = { filename };
    int res = lua_cpcall(m_L, C::call, &p);
    if(res != 0)
    {
        throw(LuaError(m_L));
    }
}


void DataManager::loadData(const QString& filename) throw(LuaError)
{
    struct C
    {
        QString name;
        static int call(lua_State* L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, -1));
            lua_getglobal(L, "loadAll");        // loadAll
            lua_pushstring(L, p->name.toAscii());// loadAll - name
            lua_call(L, 1, 0);                  // table - result
            lua_getglobal(L, "World");          // World
            lua_pop(L, 1);                      // --

            return 0;
        }
    } p = { filename };
    int res = lua_cpcall(m_L, C::call, &p);
    if(res != 0)
    {
        throw(LuaError(m_L));
    }
}


void DataManager::setKeyframeFromRef(int keyframeRef, float value)
{
    assert(NULL != m_L);

    struct C
    {
        int keyRef;
        float value;
        static int call(lua_State* L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, 1));
            lua_rawgeti(L, LUA_REGISTRYINDEX, p->keyRef);
            lua_pushnumber(L, p->value);
            lua_setfield(L, -2, "value");
            lua_pop(L, 1); /* << keyref */
            return 0;
        }
    } p = { keyframeRef, value };
    try
    {
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() <<std::endl;
    }
}


int DataManager::addKeyframe(int avarRef, int index, float value)
{
    assert(NULL != m_L);

    struct C
    {
        int avarRef;
        int frame;
        float value;
        int keyRef;
        static int call(lua_State* L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, 1));
            lua_rawgeti(L, LUA_REGISTRYINDEX, p->avarRef);      // Avar
            lua_getfield(L, -1, "addKeyframe");                 // Avar - addKeyframe
            lua_pushvalue(L, -2);   // self                     // Avar - addKeyframe - Avar
            lua_pushinteger(L, p->frame);                       // Avar - addKeyframe - Avar - frame
            lua_pushnumber(L, p->value);                        // Avar - addKeyframe - Avar - frame - value
            lua_call(L, 3, 1);                                  // Avar - kf
            p->keyRef = luaL_ref(L, LUA_REGISTRYINDEX);         // Avar
            lua_pop(L, 1);                                      // --
            return 0;
        }
    } p = { avarRef, index, value, 0 };
    try
    {
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() <<std::endl;
    }
    return p.keyRef;
}

float DataManager::getKeyframeFromRef(int keyframeRef)
{
    try
    {
        struct C
        {
            int keyRef;
            float value;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, 1));
                lua_rawgeti(L, LUA_REGISTRYINDEX, p->keyRef);
                lua_getfield(L, -1, "value");
                p->value = lua_tonumber(L, -1);
                lua_pop(L, 2); /* << value << keyref */
                return 0;
            }
        } p = { keyframeRef, 0.0f };
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
        return p.value;
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() <<std::endl;
        return 0.0f;
    }
}


QString DataManager::getAvarNameFromRef(int avarRef)
{
    struct C
    {
        int avarRef;
        QString name;
        static int call(lua_State* L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, 1));
            lua_rawgeti(L, LUA_REGISTRYINDEX, p->avarRef);
            lua_getfield(L, -1, "name");
            p->name = lua_tostring(L, -1);
            lua_pop(L, 2);  /* << name << ref*/
            return 0;
        }
    } p = { avarRef, "" };
    try
    {
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
        return p.name;
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() <<std::endl;
    }
    return "";
}

void DataManager::getKeyframesFromRef(int avarRef, QList<Keyframe>& kfs)
{
    struct C
    {
        int avarRef;
        QList<Keyframe>& keyframes;
        QString name;
        static int call(lua_State* L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, 1));
            lua_rawgeti(L, LUA_REGISTRYINDEX, p->avarRef);
            lua_getfield(L, -1, "name");
            p->name = lua_tostring(L, -1);
            lua_pop(L, 1);  /* << name */
            lua_getfield(L, -1, "keyframes");
            lua_pushnil(L); /* the first key */
            while(lua_next(L, -2) != 0)
            {
                lua_getfield(L, -1, "frame");
                int frame = lua_tointeger(L, -1);
                lua_getfield(L, -2, "value");
                float value = lua_tonumber(L, -1);
                lua_pop(L, 2); // << value << frame
                int ref = luaL_ref(L, LUA_REGISTRYINDEX);
                Keyframe kf = { ref, frame, value };
                p->keyframes << kf;
            }
            lua_pop(L, 2); /* << keyframes << ref */
            return 0;
        }
    } p = { avarRef, kfs, "" };
    try
    {
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() <<std::endl;
    }
}

void DataManager::deleteKeyframeFromRef(int avarRef, int keyframeRef)
{
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
        } p = { avarRef, keyframeRef };
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
}


QString DataManager::nodeNameFromRef(int nodeRef)
{
    try
    {
        struct C
        {
            QString nodeName;
            int     objref;
            static int call(lua_State *L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, 1));
                lua_rawgeti(L, LUA_REGISTRYINDEX, p->objref);
                if(lua_isnil(L, -1))
                {
                    lua_pushstring(L, "Error, item is nil!");
                    lua_error(L);
                }
                lua_getfield(L, -1, "name");
                const char* name = lua_tostring(L, -1);
                p->nodeName = name;
                lua_pop(L, 1);  /* << name */
                return 0;
            }
        } p = { "", nodeRef };
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
        return p.nodeName;
    }
    catch(LuaError& e)
    {
        QMessageBox box;
        box.setText(e.what());
        box.exec();
        return "";
    }
}


QString DataManager::nodeSourceFromRef(int nodeRef)
{
    try
    {
        struct C
        {
            QString nodeSource;
            int     objref;
            static int call(lua_State *L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, 1));
                lua_rawgeti(L, LUA_REGISTRYINDEX, p->objref);
                if(lua_isnil(L, -1))
                {
                    lua_pushstring(L, "Error, item is nil!");
                    lua_error(L);
                }
                lua_getfield(L, -1, "bodySource");
                const char* source = lua_tostring(L, -1);
                p->nodeSource = source;
                lua_pop(L, 1);  /* << name */
                return 0;
            }
        } p = { "", nodeRef };
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
        return p.nodeSource;
    }
    catch(LuaError& e)
    {
        QMessageBox box;
        box.setText(e.what());
        box.exec();
        return "";
    }
}


void DataManager::setNodeSourceFromRef(int nodeRef, const QString& source) throw(LuaError)
{
    if(nodeRef != LUA_NOREF)
    {
        struct C
        {
            QString source;
            int objref;
            static int call(lua_State *L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, 1));
                lua_rawgeti(L, LUA_REGISTRYINDEX, p->objref);
                lua_getfield(L, -1, "setBody"); // Get the setBody function.
                lua_pushvalue(L, -2);   // Push self.
                lua_pushstring(L, p->source.toAscii()); // Push new body text.
                lua_call(L, 2, 0);
                lua_pop(L, 1);  /* << item */
                return 0;
            }
        } p = { source.toAscii(), nodeRef };
        int res = lua_cpcall(m_L, C::call, &p);
        if( res != 0)
        {
            throw(LuaError(m_L));
        }
    }
}


void DataManager::getNodeAvarsFromRef(int nodeRef, QList<int>& avarRefs) throw(LuaError)
{
    struct C
    {
        QList<int>& avars;
        int     objref;
        static int call(lua_State *L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, 1));
            lua_rawgeti(L, LUA_REGISTRYINDEX, p->objref);
            if(lua_isnil(L, -1))
            {
                lua_pushstring(L, "Error, item is nil!");
                lua_error(L);
            }
            lua_getfield(L, -1, "avars");
            if(!lua_isnil(L, -1))
            {
                lua_pushnil(L); /* the first key */
                while(lua_next(L, -2) != 0)
                {
                    /* 'key' at -2, 'value' at -1 */
                    p->avars << luaL_ref(L, LUA_REGISTRYINDEX); /* << value */
                }
            }
            lua_pop(L, 1);  /* << avars  */
            return 0;
        }
    } p = { avarRefs, nodeRef };
    int res = lua_cpcall(m_L, C::call, &p);
    if(res != 0)
        throw(LuaError(m_L));
}


void DataManager::getCameraNodeRefs(QList<int>& refs)
{
    try
    {
        struct C
        {
            QList<int>& refs;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, -1));
                // Now fill in the cameras from the Lua state.
                lua_getglobal(L, "Cameras");                         // Cameras
                lua_pushnil(L); /* the first key */                  // Cameras - nil
                while(lua_next(L, -2) != 0)                          // Cameras - key - value
                {
                    /* 'key' at -2, 'value' at -1 */
                    int nodeRef = luaL_ref(L, LUA_REGISTRYINDEX);    // Cameras - key
                    p->refs.append(nodeRef);
                }
                lua_pop(L, 1);                              // --
                return 0;
            }
        } p = { refs };
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
}

void DataManager::setStartFrame(int frame)
{
    try
    {
        struct C
        {
            int frame;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, -1));
                lua_getglobal(L, "World");          // World
                if(!lua_isnil(L, -1))
                {
                    lua_pushinteger(L, p->frame);       // World - frame
                    lua_setfield(L, -2, "startTime");   // World
                }
                lua_pop(L, 1);                      // --
                return 0;
            }
        } p = { frame };
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
}


void DataManager::setEndFrame(int frame)
{
    try
    {
        struct C
        {
            int frame;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, -1));
                lua_getglobal(L, "World");          // World
                if(!lua_isnil(L, -1))
                {
                    lua_pushinteger(L, p->frame);       // World - frame
                    lua_setfield(L, -2, "endTime");   // World
                }
                lua_pop(L, 1);                      // --
                return 0;
            }
        } p = { frame };
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
}

int DataManager::getStartFrame()
{
    try
    {
        struct C
        {
            int frame;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, -1));
                lua_getglobal(L, "World");          // World
                if(!lua_isnil(L, -1))
                {
                    lua_getfield(L, -1, "startTime");   // World - startTime
                    p->frame = lua_tointeger(L, -1);
                    lua_pop(L, 1);                      // World
                }
                lua_pop(L, 1);                      // --
                return 0;
            }
        } p = { 0 };
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
        return p.frame;
    }
    catch(std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
    }
    return 0;
}


int DataManager::getEndFrame()
{
    try
    {
        struct C
        {
            int frame;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, -1));
                lua_getglobal(L, "World");          // World
                if(!lua_isnil(L, -1))
                {
                    lua_getfield(L, -1, "endTime");   // World - endTime
                    p->frame = lua_tointeger(L, -1);
                    lua_pop(L, 1);                      // World
                }
                lua_pop(L, 1);                      // --
                return 0;
            }
        } p = { 0 };
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));
        return p.frame;
    }
    catch(std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
    }
    return 0;
}


void DataManager::renderRenderMan()
{
    try
    {
        struct C
        {
            static int call(lua_State* L)
            {
                //C* p = static_cast<C*>(lua_touserdata(L, 1));
                // Create a new RenderMan renderer
                lua_getglobal(L, "RenderMan");      // RenderMan
                lua_getfield(L, -1, "create");      // RenderMan - create
                lua_pushvalue(L, -2);   // self     // RenderMan - create - GLRenderer
                lua_pushstring(L, "test");          // RenderMan - create - GLRenderer - name
                lua_call(L, 2, 1);                  // RenderMan - newrenderer
                lua_getfield(L, -1, "renderIt");    // RenderMan - newrenderer - renderIt
                lua_pushvalue(L, -2); // self       // RenderMan - newrenderer - renderIt - newrenderer
                lua_newtable(L);                    // RenderMan - newrenderer - renderIt - newrenderer - table
                lua_getglobal(L, "World");          // RenderMan - newrenderer - renderIt - newrenderer - table - World
                lua_setfield(L, -2, "world");       // RenderMan - newrenderer - renderIt - newrenderer - table
                lua_getglobal(L, "Cameras");        // RenderMan - newrenderer - renderIt - newrenderer - table - Cameras
                lua_getfield(L, -1, "main");        // RenderMan - newrenderer - renderIt - newrenderer - table - Cameras - main
                lua_setfield(L, -3, "camera");      // RenderMan - newrenderer - renderIt - newrenderer - table - Cameras
                lua_pop(L, 1);                      // RenderMan - newrenderer - renderIt - newrenderer - table
                lua_pushinteger(L, 320);            // RenderMan - newrenderer - renderIt - newrenderer - table - 320
                lua_setfield(L, -2, "xres");        // RenderMan - newrenderer - renderIt - newrenderer - table
                lua_pushinteger(L, 240);            // RenderMan - newrenderer - renderIt - newrenderer - table - 240
                lua_setfield(L, -2, "yres");        // RenderMan - newrenderer - renderIt - newrenderer - table
                lua_call(L, 2, 0);                  // RenderMan - newrenderer
                lua_pop(L, 2);                      // --
                return 0;
            }
        } p;
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));

    }
    catch(std::exception & e)
    {
        std::cerr << e.what() <<std::endl;
    }
}


