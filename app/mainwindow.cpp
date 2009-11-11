#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "avarlistmodel.h"
#include "scenetreemodel.h"
#include "scenetreeitem.h"

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

    QStringList avarsTest;
    avarsTest << "radius" << "zmin" << "zmax" << "thetamax";

    QAbstractItemModel* model = new AvarListModel(avarsTest);
    QTableView* view = new QTableView;
    view = ui->avarTableView;
    view->setModel(model);

    SceneTreeModel* sceneModel = new SceneTreeModel(L);
    QTreeView* sceneTreeView = new QTreeView;
    sceneTreeView = ui->sceneTreeView;
    sceneTreeView->setModel(sceneModel);

    QObject::connect(sceneTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(selectModel(QModelIndex)));

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
        static int call(lua_State *L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, 1));
            lua_getglobal(L, p->type.toAscii());
            lua_getfield(L, -1, p->container.toAscii());
            lua_getfield(L, -1, p->name.toAscii());
            lua_getfield(L, -1, "bodySource");
            const char* source = lua_tostring(L, -1);
            p->source = source;
            lua_pop(L, 4);  /* << bodySource << item << container << type */
            return 0;
        }
    } p = { type, container, name };
    int res = lua_cpcall(L, C::call, &p);
    if(res != 0)
        mainEditor->clear();
    else
        mainEditor->setText(p.source);
}
