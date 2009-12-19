#ifndef DATAMANAGER_H
#define DATAMANAGER_H

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}


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

    void setKeyframeFromRef(int keyframeRef, float value);
    int addKeyframe(int avarRef, int index, float value);
    float getKeyframe(int keyframeRef);

 private:
    DataManager()   {}
    DataManager(const DataManager&) {}
    ~DataManager()  {}

    lua_State*  m_L;
    static DataManager* m_pInstance;
};

#endif // DATAMANAGER_H
