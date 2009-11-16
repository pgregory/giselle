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
    : QMainWindow(parent), ui(new Ui::MainWindow), L(L), m_currentObjectRef(LUA_REFNIL)
{
    ui->setupUi(this);

    ui->graphicsView->initialiseLua(L);

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

    QObject::connect(ui->timeSlider, SIGNAL(valueChanged(int)), ui->graphicsView, SLOT(timeChanged(int)));

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
        QList<int> avars;
        int     objref;
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
                p->avars << luaL_ref(L, LUA_REGISTRYINDEX); /* << value */
            }
            lua_pop(L, 1);  /* << avars  */

            p->objref = luaL_ref(L, LUA_REGISTRYINDEX);    /* << item */
            lua_pop(L, 2);  /* << container << type */
            return 0;
        }
    } p = { type, container, name, "", QList<int>() };
    int res = lua_cpcall(L, C::call, &p);
    if(res != 0)
        mainEditor->clear();
    else
    {
        mainEditor->setText(p.source);
        currentType = type;
        currentContainer = container;
        currentName = name;
        luaL_unref(L, LUA_REGISTRYINDEX, m_currentObjectRef);
        m_currentObjectRef = p.objref;
    }

    QAbstractItemModel* model = new AvarListModel(L, p.avars);
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
