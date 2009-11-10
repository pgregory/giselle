#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "avarlistmodel.h"
#include "scenetreemodel.h"

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

    lua_getglobal(L, "Model");
    lua_getfield(L, -1, "models");
    lua_pushnil(L); /* the first key */
    if(lua_next(L, -2) != 0)
    {
        /* 'key' at -2, 'value' at -1 */
        lua_getfield(L, -1, "bodySource");
        const char* name = lua_tostring(L, -1);
        mainEditor->setText(name);
        lua_pop(L, 2);
    }
    lua_pop(L, 2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectModel(const QModelIndex &index)
{
    std::cerr << "Row: " << index.row() << " Column: " << index.column() << std::endl;
    lua_getglobal(L, "Model");
    lua_getfield(L, -1, "models");
    lua_pushnil(L); /* the first key */
    int rowNo = index.row();
    while(lua_next(L, -2) != 0 && --rowNo >= 0)
        lua_pop(L, 1);
    /* 'key' at -2, 'value' at -1 */
    lua_getfield(L, -1, "bodySource");
    const char* name = lua_tostring(L, -1);
    mainEditor->setText(name);
    lua_pop(L, 2);
}
