#include <QList>

#include "avarlistmodel.h"
#include "luaerror.h"

AvarListModel::AvarListModel(lua_State* L, const QList<int>& avars, QObject* parent) :
            QAbstractTableModel(parent), m_L(L)
{
    // Build a list of AvarItems from the list of avars passed.
    float max = 0.0f, min = 0.0f;
    for(QList<int>::const_iterator av = avars.begin(), end = avars.end(); av != end; ++av)
    {
        struct C
        {
            int avarRef;
            QList<QPair<float, float> > keyframes;
            QString name;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, 1));
                lua_rawgeti(L, LUA_REGISTRYINDEX, p->avarRef);
                lua_getfield(L, -1, "name");
                p->name = lua_tostring(L, -1);
                lua_pop(L, 1);  /* << name */
                lua_getfield(L, -1, "keyframes");
                QList<QPair<float, float> > keys;
                lua_pushnil(L); /* the first key */
                while(lua_next(L, -2) != 0)
                {
                    lua_getfield(L, -1, "time");
                    float time = lua_tonumber(L, -1);
                    lua_pop(L, 1); /* << time */
                    lua_getfield(L, -1, "value");
                    float value = lua_tonumber(L, -1);
                    p->keyframes << qMakePair(time, value);
                    lua_pop(L, 2); /* << kfvalue << value (leave key for next iteration) */
                }
                lua_pop(L, 2); /* << keyframes << ref */
                return 0;
            }
        } p = { *av, QList<QPair<float, float> >() };
        try
        {
            int res = lua_cpcall(L, C::call, &p);
            if(res != 0)
                throw(LuaError(L));
        }
        catch(std::exception& e)
        {
            std::cerr << e.what() <<std::endl;
        }

        avarList << AvarListItem(m_L, *av, p.name, p.keyframes);
        // Work out how many columns by looking at all the avars.
        QPair<float, float> kf;
        foreach(kf, p.keyframes)
        {
            max = qMax(max, kf.first);
            min = qMin(min, kf.first);
        }
    }
    _startFrame = qRound(min);
    _endFrame = qRound(max);
    _maxColumns = (_endFrame - _startFrame) + 1;
}

int AvarListModel::rowCount(const QModelIndex& parent) const
{
    return avarList.count();
}

int AvarListModel::columnCount(const QModelIndex& parent) const
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
        QList<QPair<float, float> >::const_iterator kf = av.keyframes().begin(), end = av.keyframes().end();
        while(kf->first < index.column() && kf != end)
            ++kf;
        if(kf != end && kf->first <= index.column())
            return kf->second;
        else
            return QVariant();
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

    const AvarListItem& av = avarList.at(index.row());
    QList<QPair<float, float> >::const_iterator kf = av.keyframes().begin(), end = av.keyframes().end();
    while(kf->first < index.column() && kf != end)
        ++kf;
    if(kf != end && kf->first <= index.column())
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    else
        return Qt::ItemIsEnabled;
}


bool AvarListModel::setData(const QModelIndex &index, const QVariant &value, int role)
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
