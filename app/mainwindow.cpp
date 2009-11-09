#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "avarlistmodel.h"
#include "scenetreemodel.h"


MainWindow::MainWindow(lua_State *L, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
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
}

MainWindow::~MainWindow()
{
    delete ui;
}
