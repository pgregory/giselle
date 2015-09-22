#include <QList>
#include <QVector>
#include <QColor>
#include <QMessageBox>

#include "avarlistmodel.h"
#include "luaerror.h"
#include "datamanager.h"

AvarListModel::AvarListModel(const QList<int>& avars, int startFrame, int endFrame, QObject* parent) :
            QAbstractTableModel(parent), m_currentTime(0)
{
    _startFrame = startFrame;
    _endFrame = endFrame;
    int i;
    foreach(i, m_avarRefs)
        DataManager::instance().releaseRef(i);
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
    else if(role == Qt::BackgroundRole)
    {
        if(index.column() == m_currentTime)
            return QColor(Qt::green);
        else
            return QColor(Qt::white);
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
    // \todo: Should be able to pass 'index' here, but for some reason it
    // doesn't update properly if I do, so until I find out why, force a complete refresh.
    emit dataChanged(QModelIndex(), QModelIndex());
    return true;
}

void AvarListModel::startFrameChanged(int start)
{
    _startFrame = start;
    _maxColumns = (_endFrame-_startFrame)+1;
    //reset();
    populateModel();
}


void AvarListModel::endFrameChanged(int end)
{
    _endFrame = end;
    _maxColumns = (_endFrame-_startFrame)+1;
    //reset();
    populateModel();
}


void AvarListModel::timeChanged(int time)
{
    QModelIndex olds = index(0, m_currentTime);
    QModelIndex olde = index(rowCount(), m_currentTime);
    m_currentTime = time;
    QModelIndex news = index(0, m_currentTime);
    QModelIndex newe = index(rowCount(), m_currentTime);
    emit dataChanged(olds, olde);
    emit dataChanged(news, newe);
}


void AvarListModel::populateModel()
{
    avarList.clear();

    try
    {
        _maxColumns = 0;
        // Build a list of AvarItems from the list of avars passed.
        for(QList<int>::const_iterator av = m_avarRefs.begin(), end = m_avarRefs.end(); av != end; ++av)
        {
            QString name = DataManager::instance().getAvarNameFromRef(*av);
            QList<Keyframe> kfs;
            DataManager::instance().getKeyframesFromRef(*av, kfs);

            // Now build a list of ints, over the span, filling in valid keyframes where available.
            QVector<int> keyframes;

            keyframes.insert(0, (_endFrame-_startFrame)+1, LUA_NOREF);
            if(kfs.size() > 0)
            {
                Keyframe kf;
                foreach(kf, kfs)
                {
                    if(kf.frame >= _startFrame && kf.frame <= _endFrame)
                        keyframes[kf.frame-_startFrame] = kf.ref;
                }
            }
            _maxColumns = (_endFrame - _startFrame) + 1;
            avarList << AvarListItem(*av, name, keyframes);
        }
    }
    catch(LuaError& e)
    {
        QMessageBox box;
        box.setText(e.what());
        box.exec();
    }
}


const AvarListItem& AvarListModel::avarListItem(QModelIndex& index) const
{
    const AvarListItem& av = avarList.at(index.row());
    return av;
}


int AvarListModel::keyFrameRef(QModelIndex& index) const
{
    const AvarListItem& av = avarListItem(index);
    return av.getKeyframeRef(index.column());
}

void AvarListModel::clearFrame(QModelIndex& index)
{
    const AvarListItem& av = avarListItem(index);
    int avarref = av.avarRef();
    int keyref = av.getKeyframeRef(index.column());
    DataManager::instance().deleteKeyframeFromRef(avarref, keyref);
    refresh();
    emit dataChanged(index, index);
}
