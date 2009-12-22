#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QList>
#include <QString>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}


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

    // Avar related methods.
    void getKeyframesFromRef(int avarRef, QList<Keyframe>& kfs);
    QString getAvarNameFromRef(int avarRef);
    void setKeyframeFromRef(int keyframeRef, float value);
    float getKeyframeFromRef(int keyframeRef);
    int addKeyframe(int avarRef, int index, float value);
    void deleteKeyframeFromRef(int avarRef, int keyframeRef);

 private:
    DataManager()   {}
    DataManager(const DataManager&) {}
    ~DataManager()  {}

    lua_State*  m_L;
    static DataManager* m_pInstance;
};

#endif // DATAMANAGER_H
