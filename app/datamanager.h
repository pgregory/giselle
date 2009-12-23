#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QList>
#include <QString>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include "luaerror.h"

typedef struct _Keyframe
{
    int ref;
    int frame;
    float value;
} Keyframe;

class DataManager
{
 public:

    static DataManager& instance()
    {
        static DataManager instance;
        return instance;
    }

    void setLuaState(lua_State* L)
    {
        m_L = L;
    }
    lua_State* getLuaState() const
    {
        return m_L;
    }

    // General methods.
    int cloneRef(int ref);
    void releaseRef(int ref);
    bool refsEqual(int refa, int refb) throw(LuaError);
    void runCommand(const QString& command) throw(LuaError);
    void saveData(const QString& filename) throw(LuaError);
    void loadData(const QString& filename) throw(LuaError);

    // Avar related methods.
    void getKeyframesFromRef(int avarRef, QList<Keyframe>& kfs) throw(LuaError);
    QString getAvarNameFromRef(int avarRef) throw(LuaError);
    void setKeyframeFromRef(int keyframeRef, float value) throw(LuaError);
    float getKeyframeFromRef(int keyframeRef) throw(LuaError);
    int addKeyframe(int avarRef, int index, float value) throw(LuaError);
    void deleteKeyframeFromRef(int avarRef, int keyframeRef) throw(LuaError);

    // Node methods
    QString nodeNameFromRef(int nodeRef) throw(LuaError);
    QString nodeSourceFromRef(int nodeRef) throw(LuaError);
    void setNodeSourceFromRef(int nodeRef, const QString& source) throw(LuaError);
    void getNodeAvarsFromRef(int nodeRef, QList<int>& avarRefs) throw(LuaError);
    void getCameraNodeRefs(QList<int>& refs) throw(LuaError);
    void getNodeChildrendRefsFromRef(int nodeRef, QList<int>& children) throw(LuaError);

    // World methods
    int getWorldRef() throw(LuaError);
    void setStartFrame(int frame) throw(LuaError);
    void setEndFrame(int frame) throw(LuaError);
    int getStartFrame() throw(LuaError);
    int getEndFrame() throw(LuaError);

    // Rendering methods
    void renderRenderMan() throw(LuaError);
    int createGLRendererRef() throw(LuaError);
    void renderGL(int rendererRef, int frame, const QString& cameraName) throw(LuaError);

 private:
    DataManager()   {}
    DataManager(const DataManager&) {}
    ~DataManager()  {}

    lua_State*  m_L;
    static DataManager* m_pInstance;
};

#endif // DATAMANAGER_H
