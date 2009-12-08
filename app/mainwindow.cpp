#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "avarlistmodel.h"
#include "avartableview.h"
#include "scenetreemodel.h"
#include "scenetreeitem.h"

#include <QCoreApplication>
#include <QMap>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QCloseEvent>

extern "C" {
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
}

#include "luaerror.h"


MainWindow::MainWindow(lua_State *L, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), L(L), m_currentObjectRef(LUA_REFNIL), m_currentAvarModel(0)
{
    QCoreApplication::setApplicationName("TheAnimator");
    QCoreApplication::setOrganizationName("Aqsis Team");
    QCoreApplication::setOrganizationDomain("aqsis.org");

    ui->setupUi(this);

    ui->graphicsView->initialiseLua(L);
    ui->avarTableView->initialiseLua(L);

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

    QObject::connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(save()));
    QObject::connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(load()));

    QObject::connect(ui->timeSlider, SIGNAL(valueChanged(int)), ui->avarTableView, SLOT(timeChanged(int)));
    QObject::connect(ui->lockCheck, SIGNAL(stateChanged(int)), ui->avarTableView, SLOT(lock(int)));

    mainEditor = new QTextEdit;
    mainEditor = ui->mainEditor;
    highlighter = new LuaHighlighter(mainEditor->document());

    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectModel(const QModelIndex &index)
{
    if(!index.isValid())
        return;

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
    ui->avarTableView->setModel(model);
    QObject::connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(avarsChanged(QModelIndex, QModelIndex)));
    QObject::connect(ui->timeMin, SIGNAL(valueChanged(int)), model, SLOT(startFrameChanged(int)));
    QObject::connect(ui->timeMax, SIGNAL(valueChanged(int)), model, SLOT(endFrameChanged(int)));
    QObject::connect(ui->timeSlider, SIGNAL(valueChanged(int)), model, SLOT(timeChanged(int)));
    delete(m_currentAvarModel);
    m_currentAvarModel = model;
}

void MainWindow::acceptChanges()
{
    if(m_currentObjectRef != LUA_NOREF)
    {
        try
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
                    lua_call(L, 2, 0);
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
        catch(std::exception& e)
        {
            QMessageBox msgBox;
            msgBox.setText(e.what());
            msgBox.exec();
        }
    }
}

void MainWindow::doRender()
{
    try
    {
        struct C
        {
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, 1));
                // Create a new RenderMan renderer
                lua_getglobal(L, "RenderMan");      // RenderMan
                lua_getfield(L, -1, "create");      // RenderMan - create
                lua_pushvalue(L, -2);   // self     // RenderMan - create - GLRenderer
                lua_pushstring(L, "test");          // RenderMan - create - GLRenderer - name
                lua_call(L, 2, 1);                  // RenderMan - newrenderer
                lua_getfield(L, -1, "renderIt");    // RenderMan - newrenderer - renderIt
                lua_pushvalue(L, -2); // self       // RenderMan - newrenderer - renderIt - newrenderer
                lua_newtable(L);                    // RenderMan - newrenderer - renderIt - newrenderer - table
                lua_getglobal(L, "World");          // RenderMan - newrenderer - renderIt - newrenderer - table - World
                lua_setfield(L, -2, "world");       // RenderMan - newrenderer - renderIt - newrenderer - table
                lua_getglobal(L, "Cameras");        // RenderMan - newrenderer - renderIt - newrenderer - table - Cameras
                lua_getfield(L, -1, "main");        // RenderMan - newrenderer - renderIt - newrenderer - table - Cameras - main
                lua_setfield(L, -3, "camera");      // RenderMan - newrenderer - renderIt - newrenderer - table - Cameras
                lua_pop(L, 1);                      // RenderMan - newrenderer - renderIt - newrenderer - table
                lua_getglobal(L, "World");          // RenderMan - newrenderer - renderIt - newrenderer - table - World
                lua_getfield(L, -1, "startTime");   // RenderMan - newrenderer - renderIt - newrenderer - table - World - startTime
                lua_setfield(L, -2, "start");       // RenderMan - newrenderer - renderIt - newrenderer - table - World
                lua_getfield(L, -2, "endTime");     // RenderMan - newrenderer - renderIt - newrenderer - table - World - endTime
                lua_setfield(L, -2, "stop");        // RenderMan - newrenderer - renderIt - newrenderer - table - World
                lua_pop(L, 1);                      // RenderMan - newrenderer - renderIt - newrenderer - table
                lua_call(L, 2, 0);                  // RenderMan - newrenderer
                lua_pop(L, 2);                      // --
                return 0;
            }
        } p;
        int res = lua_cpcall(L, C::call, &p);
        if(res != 0)
            throw(LuaError(L));

    }

    catch(std::exception & e)
    {
        std::cerr << e.what() <<std::endl;
    }
}

void MainWindow::avarsChanged(const QModelIndex&, const QModelIndex&)
{
    ui->graphicsView->updateGL();
}

void MainWindow::startFrameChanged(int start)
{
    // Set the time slider range.
    ui->timeSlider->setMinimum(start);
    // Update the value stored on World in Lua
    try
    {
        struct C
        {
            int frame;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, -1));
                lua_getglobal(L, "World");          // World
                if(!lua_isnil(L, -1))
                {
                    lua_pushinteger(L, p->frame);       // World - frame
                    lua_setfield(L, -2, "startTime");   // World
                }
                lua_pop(L, 1);                      // --
                return 0;
            }
        } p = { start };
        int res = lua_cpcall(L, C::call, &p);
        if(res != 0)
            throw(LuaError(L));
    }
    catch(std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
    }
}

void MainWindow::endFrameChanged(int end)
{
    // Update the time slider range.
    ui->timeSlider->setMaximum(end);
    // Update the value stored on World in Lua
    try
    {
        struct C
        {
            int frame;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, -1));
                lua_getglobal(L, "World");          // World
                if(!lua_isnil(L, -1))
                {
                    lua_pushinteger(L, p->frame);       // World - frame
                    lua_setfield(L, -2, "endTime");     // World
                }
                lua_pop(L, 1);                      // --
                return 0;
            }
        } p = { end };
        int res = lua_cpcall(L, C::call, &p);
        if(res != 0)
            throw(LuaError(L));
    }
    catch(std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
    }
}


void MainWindow::runCommand()
{
    QString command = ui->lineEdit->text();
    try
    {
        int res = luaL_dostring(L, command.toAscii());
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
    selectModel(ui->sceneTreeView->currentIndex());
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

void MainWindow::save()
{
    QString name = QFileDialog::getSaveFileName(this, "Save File", "", "Animator Files(*.ta)");
    try
    {
        struct C
        {
            QString name;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, -1));
                lua_getglobal(L, "saveAll");        // saveAll
                lua_pushstring(L, p->name.toAscii());// saveAll - name
                lua_call(L, 1, 0);                  // --

                return 0;
            }
        } p = { name };
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
    }
}


void MainWindow::load()
{
    QString name = QFileDialog::getOpenFileName(this, "Load File", "", "Animator Files(*.ta)");
    try
    {
        struct C
        {
            QString name;
            int     startframe;
            int     endframe;
            static int call(lua_State* L)
            {
                C* p = static_cast<C*>(lua_touserdata(L, -1));
                lua_getglobal(L, "loadAll");        // loadAll
                lua_pushstring(L, p->name.toAscii());// loadAll - name
                lua_call(L, 1, 0);                  // table - result
                lua_getglobal(L, "World");          // World
                lua_getfield(L, -1, "startTime");   // World - startTime
                lua_getfield(L, -2, "endTime");     // World - startTime - endTime
                p->startframe = lua_tointeger(L, -2);
                p->endframe = lua_tointeger(L, -1);
                lua_pop(L, 3);                      // --

                return 0;
            }
        } p = { name };
        int res = lua_cpcall(L, C::call, &p);
        if(res != 0)
        {
            throw(LuaError(L));
        }
        ui->timeMin->setValue(p.startframe);
        ui->timeMax->setValue(p.endframe);
    }
    catch(std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
    }
    populateTree();
}


void MainWindow::writeSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("state", saveState());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(815, 600)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    restoreState(settings.value("state", QByteArray()).toByteArray());
    settings.endGroup();
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    writeSettings();
    event->accept();
}
