#include <QList>
#include <QVector>

#include "avarlistmodel.h"
#include "luaerror.h"

typedef struct _KeyFrame
{
    int ref;
    int frame;
    float value;
} KeyFrame;

AvarListModel::AvarListModel(lua_State* L, const QList<int>& avars, int startFrame, int endFrame, QObject* parent) :
            QAbstractTableModel(parent), m_L(L)
{
    _startFrame = startFrame;
    _endFrame = endFrame;
    int i;
    foreach(i, m_avarRefs)
        luaL_unref(m_L, LUA_REGISTRYINDEX, i);
    m_avarRefs = avars;
    populateModel();
}

int AvarListModel::rowCount(const QModelIndex& /*parent*/) const
{
    return avarList.count();
}

int AvarListModel::columnCount(const QModelIndex& /*parent*/) const
{
    return _maxColumns;
}


QVariant AvarListModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row() >= avarList.size())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        const AvarListItem& av = avarList.at(index.row());
        return av.getKeyframeValue(index.column());
    }
    else
        return QVariant();
}

QVariant AvarListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal)
    {
        if(avarList.size() > 0)
            return _startFrame + section;
        else
            return QVariant();
    }
    else
    {
        if(section >= avarList.size())
            return QVariant();
        else
            return avarList.at(section).name();
    }
}


Qt::ItemFlags AvarListModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;

    if(index.row() >= avarList.size())
        return Qt::ItemIsEnabled;

//    const AvarListItem& av = avarList.at(index.row());
//    if(av.getKeyframeValue(index.column() + _startFrame).isValid())
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
//    else
//        return Qt::ItemIsEnabled;
}


bool AvarListModel::setData(const QModelIndex &index, const QVariant &value, int /*role*/)
{
    if(!index.isValid())
        return false;

    if(index.row() >= avarList.size())
        return false;

    AvarListItem& av = avarList[index.row()];
    av.setKeyframe(index.column() + _startFrame, value.toDouble());
    emit dataChanged(index, index);
    return true;
}

void AvarListModel::startFrameChanged(int start)
{
    _startFrame = start;
    _maxColumns = (_endFrame-_startFrame)+1;
    reset();
    populateModel();
}


void AvarListModel::endFrameChanged(int end)
{
    _endFrame = end;
    _maxColumns = (_endFrame-_startFrame)+1;
    reset();
    populateModel();
}


void AvarListModel::populateModel()
{
    avarList.clear();

    _maxColumns = 0;
    // Build a list of AvarItems from the list of avars passed.
    for(QList<int>::const_iterator av = m_avarRefs.begin(), end = m_avarRefs.end(); av != end; ++av)
    {
        struct C
        {
            int avarRef;
            QList<KeyFrame> keyframes;
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
                    KeyFrame kf = { ref, frame, value };
                    p->keyframes << kf;
                }
                lua_pop(L, 2); /* << keyframes << ref */
                return 0;
            }
        } p = { *av, QList<KeyFrame>(), "" };
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

        // Now build a list of ints, over the span, filling in valid keyframes where available.
        QVector<int> keyframes;

        if(p.keyframes.size() > 0)
        {
            keyframes.insert(0, (_endFrame-_startFrame)+1, LUA_NOREF);
            KeyFrame kf;
            foreach(kf, p.keyframes)
            {
                if(kf.frame >= _startFrame && kf.frame <= _endFrame)
                    keyframes.insert(kf.frame-_startFrame, kf.ref);
            }
            _maxColumns = (_endFrame - _startFrame) + 1;
        }
        avarList << AvarListItem(m_L, *av, p.name, keyframes);
    }
}
