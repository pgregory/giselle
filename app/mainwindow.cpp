#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "avarlistmodel.h"
#include "scenetreemodel.h"
#include "scenetreeitem.h"

#include <QMap>
#include <QMessageBox>

extern "C" {
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
}

#include "luaerror.h"


MainWindow::MainWindow(lua_State *L, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), L(L), m_currentObjectRef(LUA_REFNIL)
{
    ui->setupUi(this);

    ui->graphicsView->initialiseLua(L);

    splitter = new QSplitter;
    splitter = ui->splitterEditTree;
    splitter->setStretchFactor(1,1);

    populateTree();

    QObject::connect(ui->sceneTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(selectModel(QModelIndex)));
    QObject::connect(ui->actionAccept, SIGNAL(triggered()), this, SLOT(acceptChanges()));
    QObject::connect(ui->actionRender, SIGNAL(triggered()), this, SLOT(doRender()));

    QObject::connect(ui->timeSlider, SIGNAL(valueChanged(int)), ui->graphicsView, SLOT(timeChanged(int)));
    QObject::connect(ui->timeMin, SIGNAL(valueChanged(int)), this, SLOT(startFrameChanged(int)));
    QObject::connect(ui->timeMax, SIGNAL(valueChanged(int)), this, SLOT(endFrameChanged(int)));

    QObject::connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(runCommand()));

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
    SceneTreeItem *item = static_cast<SceneTreeItem*>(index.internalPointer());
    int nodeRef = item->nodeRef();
    QMap<QString, QList<QPair<float, float> > > avarsList;

    struct C
    {
        QString source;
        QList<int> avars;
        int     objref;
        static int call(lua_State *L)
        {
            C* p = static_cast<C*>(lua_touserdata(L, 1));
            lua_rawgeti(L, LUA_REGISTRYINDEX, p->objref);
            if(lua_isnil(L, -1))
            {
                lua_pushstring(L, "Error, item is nil!");
                lua_error(L);
            }
            lua_getfield(L, -1, "bodySource");
            const char* source = lua_tostring(L, -1);
            p->source = source;
            lua_pop(L, 1);  /* << bodySource */
            lua_getfield(L, -1, "avars");
            if(!lua_isnil(L, -1))
            {
                lua_pushnil(L); /* the first key */
                while(lua_next(L, -2) != 0)
                {
                    /* 'key' at -2, 'value' at -1 */
                    p->avars << luaL_ref(L, LUA_REGISTRYINDEX); /* << value */
                }
            }
            lua_pop(L, 1);  /* << avars  */
            return 0;
        }
    } p = { "", QList<int>(), nodeRef };
    int res = lua_cpcall(L, C::call, &p);
    if(res != 0)
        mainEditor->clear();
    else
    {
        mainEditor->setText(p.source);
        luaL_unref(L, LUA_REGISTRYINDEX, m_currentObjectRef);
        // Duplicate the reference for our ownership.
        lua_rawgeti(L, LUA_REGISTRYINDEX, nodeRef);
        m_currentObjectRef = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    AvarListModel* model = new AvarListModel(L, p.avars, ui->timeMin->value(), ui->timeMax->value());
    QTableView* view = new QTableView;
    view = ui->avarTableView;
    QItemSelectionModel* old = view->selectionModel();
    view->setModel(model);
    delete old;
    QObject::connect(ui->avarTableView->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(avarsChanged(QModelIndex, QModelIndex)));
    QObject::connect(ui->timeMin, SIGNAL(valueChanged(int)), model, SLOT(startFrameChanged(int)));
    QObject::connect(ui->timeMax, SIGNAL(valueChanged(int)), model, SLOT(endFrameChanged(int)));
}

void MainWindow::acceptChanges()
{
    if(m_currentObjectRef != LUA_NOREF)
    {
        struct C
        {
            QString source;
            int objref;
            static int call(lua_State *L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, 1));
                lua_rawgeti(L, LUA_REGISTRYINDEX, p->objref);
                lua_getfield(L, -1, "setBody"); // Get the setBody function.
                lua_pushvalue(L, -2);   // Push self.
                lua_pushstring(L, p->source.toAscii()); // Push new body text.
                lua_pcall(L, 2, 0, 0);
                lua_pop(L, 1);  /* << item */
                return 0;
            }
        } p = { mainEditor->toPlainText().toAscii(), m_currentObjectRef };
        int res = lua_cpcall(L, C::call, &p);
        if( res != 0)
        {
            throw(LuaError(L));
        }
        ui->graphicsView->updateGL();
    }
}

void MainWindow::doRender()
{
}

void MainWindow::avarsChanged(const QModelIndex&, const QModelIndex&)
{
    ui->graphicsView->updateGL();
}

void MainWindow::startFrameChanged(int start)
{
    ui->timeSlider->setMinimum(start);
}

void MainWindow::endFrameChanged(int end)
{
    ui->timeSlider->setMaximum(end);
}


void MainWindow::runCommand()
{
    QString command = ui->lineEdit->text();
    try
    {
        struct C
        {
            QString comand;
            static int call(lua_State *L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, 1));
                int res = luaL_dostring(L, p->comand.toAscii());
                if(res != 0)
                    throw(LuaError(L));

                return 0;
            }
        } p = { command };
        int res = lua_cpcall(L, C::call, &p);
        if(res != 0)
        {
            throw(LuaError(L));
        }
    }
    catch(std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
        return;
    }
    populateTree();
    ui->lineEdit->clear();
}

void MainWindow::populateTree()
{
    SceneTreeModel* sceneModel = new SceneTreeModel(L);
    QItemSelectionModel* old = ui->sceneTreeView->selectionModel();
    ui->sceneTreeView->setModel(sceneModel);
    delete old;

    // Set the Models and Cameras nodes as expanded.
    ui->sceneTreeView->setExpanded(sceneModel->index(0,0,QModelIndex()), true);
    ui->sceneTreeView->setExpanded(sceneModel->index(1,0,QModelIndex()), true);
}
