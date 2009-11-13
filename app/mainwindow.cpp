#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "avarlistmodel.h"
#include "scenetreemodel.h"
#include "scenetreeitem.h"

#include <QMap>

extern "C" {
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
}


MainWindow::MainWindow(lua_State *L, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), L(L)
{
    ui->setupUi(this);

    splitter = new QSplitter;
    splitter = ui->splitterEditTree;
    splitter->setStretchFactor(1,1);

    SceneTreeModel* sceneModel = new SceneTreeModel(L);
    QTreeView* sceneTreeView = new QTreeView;
    sceneTreeView = ui->sceneTreeView;
    sceneTreeView->setModel(sceneModel);

    QObject::connect(sceneTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(selectModel(QModelIndex)));
    QObject::connect(ui->actionAccept, SIGNAL(triggered()), this, SLOT(acceptChanges()));
    QObject::connect(ui->actionRender, SIGNAL(triggered()), this, SLOT(doRender()));

    mainEditor = new QTextEdit;
    mainEditor = ui->mainEditor;
    highlighter = new LuaHighlighter(mainEditor->document());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectModel(const QModelIndex &index)
{
    QString type, container;
    SceneTreeItem *item = static_cast<SceneTreeItem*>(index.internalPointer());
    QString name = item->data(0).toString();
    QMap<QString, QList<QPair<float, float> > > avarsList;

    switch(item->type())
    {
        case MODEL:
            type = "Model";
            container = "models";
            break;
        case CAMERA:
            type = "Camera";
            container = "cameras";
            break;
        default:
            return;
    }

    struct C
    {
        QString type;
        QString container;
        QString name;
        QString source;
        QMap<QString, QList<QPair<float, float> > >& avars;
        static int call(lua_State *L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, 1));
            lua_getglobal(L, p->type.toAscii());
            lua_getfield(L, -1, p->container.toAscii());
            lua_getfield(L, -1, p->name.toAscii());
            lua_getfield(L, -1, "bodySource");
            const char* source = lua_tostring(L, -1);
            p->source = source;
            lua_pop(L, 1);  /* << bodySource */
            lua_getfield(L, -1, "avars");
            lua_pushnil(L); /* the first key */
            while(lua_next(L, -2) != 0)
            {
                /* 'key' at -2, 'value' at -1 */
                const char* name = lua_tostring(L, -2); /* key */
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
                    keys << qMakePair(time, value);
                    lua_pop(L, 2); /* << value << value [leave key for next iteration */
                }
                p->avars[name] = keys;
                lua_pop(L, 2);  /* << keyframes, pop value, leave key for next iteration */
            }
            lua_pop(L, 4);  /* << avars << item << container << type */
            return 0;
        }
    } p = { type, container, name, "", avarsList };
    int res = lua_cpcall(L, C::call, &p);
    if(res != 0)
        mainEditor->clear();
    else
    {
        mainEditor->setText(p.source);
        currentType = type;
        currentContainer = container;
        currentName = name;
    }

    QList<AvarListItem> avars;
    for(QMap<QString, QList<QPair<float, float> > >::iterator k = avarsList.begin(), end = avarsList.end(); k != end; ++k)
        avars << AvarListItem(k.key(), k.value());
    QAbstractItemModel* model = new AvarListModel(avars);
    QTableView* view = new QTableView;
    view = ui->avarTableView;
    QItemSelectionModel* old = view->selectionModel();
    view->setModel(model);
    delete old;
}

void MainWindow::acceptChanges()
{
    if(currentType != "" && currentContainer != "" && currentName != "")
    {
        struct C
        {
            QString type;
            QString container;
            QString name;
            QString source;
            static int call(lua_State *L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, 1));
                lua_getglobal(L, p->type.toAscii());    // Get the type object.
                lua_getfield(L, -1, p->container.toAscii());    // Get the container.
                lua_getfield(L, -1, p->name.toAscii()); // Get the chosen item from the container.
                lua_getfield(L, -1, "setBody"); // Get the setBody function.
                lua_pushvalue(L, -2);   // Push self.
                lua_pushstring(L, p->source.toAscii()); // Push new body text.
                lua_pcall(L, 2, 0, 0);
                lua_pop(L, 3);  /* << item << container << type */
                return 0;
            }
        } p = { currentType, currentContainer, currentName, mainEditor->toPlainText().toAscii() };
        /*int res = */lua_cpcall(L, C::call, &p);
    }
}

void MainWindow::doRender()
{
}
