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
    bool refsEqual(int refa, int refb);
    void runCommand(const QString& command) throw(LuaError);
    void saveData(const QString& filename) throw(LuaError);
    void loadData(const QString& filename) throw(LuaError);

    // Avar related methods.
    void getKeyframesFromRef(int avarRef, QList<Keyframe>& kfs);
    QString getAvarNameFromRef(int avarRef);
    void setKeyframeFromRef(int keyframeRef, float value);
    float getKeyframeFromRef(int keyframeRef);
    int addKeyframe(int avarRef, int index, float value);
    void deleteKeyframeFromRef(int avarRef, int keyframeRef);

    // Node methods
    QString nodeNameFromRef(int nodeRef);
    QString nodeSourceFromRef(int nodeRef);
    void setNodeSourceFromRef(int nodeRef, const QString& source) throw(LuaError);
    void getNodeAvarsFromRef(int nodeRef, QList<int>& avarRefs) throw(LuaError);
    void getCameraNodeRefs(QList<int>& refs);

    // World methods
    void setStartFrame(int frame);
    void setEndFrame(int frame);
    int getStartFrame();
    int getEndFrame();

    // Rendering methods
    void renderRenderMan();

 private:
    DataManager()   {}
    DataManager(const DataManager&) {}
    ~DataManager()  {}

    lua_State*  m_L;
    static DataManager* m_pInstance;
};

#endif // DATAMANAGER_H
