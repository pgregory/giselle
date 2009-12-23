#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "avarlistmodel.h"
#include "avartableview.h"
#include "scenetreemodel.h"
#include "scenetreeitem.h"
#include "luaeditor.h"
#include "datamanager.h"

#include <QCoreApplication>
#include <QMap>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QCloseEvent>

//extern "C" {
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
//}

#include "luaerror.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_currentAvarModel(0), m_sceneModel(0)
{
    QCoreApplication::setApplicationName("TheAnimator");
    QCoreApplication::setOrganizationName("Aqsis Team");
    QCoreApplication::setOrganizationDomain("aqsis.org");

    ui->setupUi(this);

    ui->graphicsView->initialiseLua(DataManager::instance().getLuaState());

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

    QObject::connect(ui->cameraCombo, SIGNAL(currentIndexChanged(QString)), ui->graphicsView, SLOT(cameraChanged(QString)));

    // Clear default tabs before connecting to make sure the slots are never called
    // with invalid widgets.
    ui->editorTabWidget->clear();
    QObject::connect(ui->editorTabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(editorTabClosed(int)));
    QObject::connect(ui->editorTabWidget, SIGNAL(currentChanged(int)), this, SLOT(editorTabChanged(int)));

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

    // Check if the model is already opened.
    for(int i = 0; i < ui->editorTabWidget->count(); ++i)
    {
        if(DataManager::instance().refsEqual(nodeRef, static_cast<LuaEditor*>(ui->editorTabWidget->widget(i))->nodeRef()))
        {
            ui->editorTabWidget->setCurrentIndex(i);
            return;
        }
    }

    QMap<QString, QList<QPair<float, float> > > avarsList;

    QString name = DataManager::instance().nodeNameFromRef(nodeRef);
    QString source = DataManager::instance().nodeSourceFromRef(nodeRef);

    LuaEditor* editor = new LuaEditor(DataManager::instance().getLuaState());
    editor->setNodeRef(nodeRef);
    editor->setText(source);
    ui->editorTabWidget->addTab(editor, name.toAscii());
    ui->editorTabWidget->setCurrentWidget(editor);

    populateAvarView(nodeRef);
}


void MainWindow::saveCode(LuaEditor* editor) throw(LuaError)
{
    DataManager::instance().setNodeSourceFromRef(editor->nodeRef(), editor->toPlainText());
    ui->graphicsView->updateGL();
    populateTree();
    populateAvarView(editor->nodeRef());
}

void MainWindow::acceptChanges()
{
    LuaEditor* editor = static_cast<LuaEditor*>(ui->editorTabWidget->currentWidget());
    try
    {
        saveCode(editor);
        editor->document()->setModified(false);
    }
    catch(std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
    }
}

void MainWindow::doRender()
{
    DataManager::instance().renderRenderMan();
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
    DataManager::instance().setStartFrame(start);
}

void MainWindow::endFrameChanged(int end)
{
    // Update the time slider range.
    ui->timeSlider->setMaximum(end);
    // Update the value stored on World in Lua
    DataManager::instance().setEndFrame(end);
}


void MainWindow::runCommand()
{
    QString command = ui->lineEdit->text();
    try
    {
        DataManager::instance().runCommand(command);
    }
    catch(std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
        return;
    }
    ui->graphicsView->updateGL();
    populateTree();
    selectModel(ui->sceneTreeView->currentIndex());
    ui->lineEdit->clear();
}

void MainWindow::populateAvarView(int nodeRef)
{
    QMap<QString, QList<QPair<float, float> > > avarsList;

    try
    {
        QList<int> avars;
        DataManager::instance().getNodeAvarsFromRef(nodeRef, avars);
        AvarListModel* model = new AvarListModel(avars, ui->timeMin->value(), ui->timeMax->value());
        ui->avarTableView->setModel(model);
        QObject::connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(avarsChanged(QModelIndex, QModelIndex)));
        QObject::connect(ui->timeMin, SIGNAL(valueChanged(int)), model, SLOT(startFrameChanged(int)));
        QObject::connect(ui->timeMax, SIGNAL(valueChanged(int)), model, SLOT(endFrameChanged(int)));
        QObject::connect(ui->timeSlider, SIGNAL(valueChanged(int)), model, SLOT(timeChanged(int)));
        delete(m_currentAvarModel);
        m_currentAvarModel = model;
    }
    catch(std::exception& e)
    {
        QMessageBox box;
        box.setText(e.what());
        box.exec();
    }

}

void MainWindow::populateTree(bool clear)
{
    if(clear)
    {
        delete m_sceneModel;
        m_sceneModel = NULL;
    }

    if(!m_sceneModel)
    {
        m_sceneModel = new SceneTreeModel(DataManager::instance().getLuaState());
        ui->sceneTreeView->setModel(m_sceneModel);

        QObject::connect(m_sceneModel, SIGNAL(rowsInserted(QModelIndex, int, int)), ui->sceneTreeView, SLOT(expand(QModelIndex)));
    }

    m_sceneModel->populateData(DataManager::instance().getLuaState());

    ui->cameraCombo->clear();

    QList<int> camRefs;
    DataManager::instance().getCameraNodeRefs(camRefs);
    int camRef;
    foreach(camRef, camRefs)
    {
        QString name = DataManager::instance().nodeNameFromRef(camRef);
        QList<QVariant> cameraData;
        cameraData << name;
        ui->cameraCombo->addItem(name, camRef);
    }

    // Set the World node as expanded.
    ui->sceneTreeView->setExpanded(m_sceneModel->index(0,0,QModelIndex()), true);
}

void MainWindow::save()
{
    QString name = QFileDialog::getSaveFileName(this, "Save File", "", "Animator Files(*.ta)");
    try
    {
        DataManager::instance().saveData(name);
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
        DataManager::instance().loadData(name);
        ui->timeMin->setValue(DataManager::instance().getStartFrame());
        ui->timeMax->setValue(DataManager::instance().getEndFrame());
    }
    catch(std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
    }
    populateTree(true);
}


void MainWindow::writeSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("state", saveState(3));
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(815, 600)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    restoreState(settings.value("state", QByteArray()).toByteArray(), 3);
    settings.endGroup();
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    // Check if any of the open editor tabs need to be saved.
    bool unsaved = false;
    for(int i = 0; i < ui->editorTabWidget->count(); ++i)
    {
        LuaEditor* editor = static_cast<LuaEditor*>(ui->editorTabWidget->widget(i));
        if(editor->document()->isModified())
            unsaved = true;
    }
    if(unsaved)
    {
        QMessageBox box;
        box.setText("There are editors open with unsaved code.");
        box.setInformativeText("Would you like to accept the changes first?");
        box.setStandardButtons(QMessageBox::SaveAll | QMessageBox::Discard | QMessageBox::Cancel);
        box.setDefaultButton(QMessageBox::SaveAll);
        int res = box.exec();

        switch(res)
        {
            case QMessageBox::SaveAll:
            {
                for(int i = 0; i < ui->editorTabWidget->count(); ++i)
                {
                    LuaEditor* editor = static_cast<LuaEditor*>(ui->editorTabWidget->widget(i));
                    if(editor->document()->isModified())
                    {
                        try
                        {
                            saveCode(editor);
                        }
                        catch(LuaError& e)
                        {
                            QMessageBox err;
                            err.setText(e.what());
                            err.exec();
                            event->ignore();
                            return;
                        }
                    }
                }
            }
            break;

            case QMessageBox::Cancel:
                event->ignore();
                return;
                break;

            case QMessageBox::Discard:
            default:
                break;
        }
    }

    for(int i = 0; i < ui->editorTabWidget->count(); ++i)
    {
        LuaEditor* editor = static_cast<LuaEditor*>(ui->editorTabWidget->widget(i));
        ui->editorTabWidget->removeTab(i);
        delete editor;
    }

    writeSettings();
    event->accept();
}


void MainWindow::editorTabClosed(int index)
{
    LuaEditor* tab = static_cast<LuaEditor*>(ui->editorTabWidget->widget(index));
    if(tab->document()->isModified())
    {
        QMessageBox box;
        box.setText("The code has been changed.");
        box.setInformativeText("Do you want to accept the changes first?");
        box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        box.setDefaultButton(QMessageBox::Save);
        int res = box.exec();

        switch(res)
        {
            case QMessageBox::Cancel:
                return;
                break;
            case QMessageBox::Save:
                try
                {
                    saveCode(tab);
                }
                catch(LuaError& e)
                {
                    QMessageBox err;
                    err.setText(e.what());
                    err.exec();
                    return;
                }
                break;
            case QMessageBox::Discard:
            default:
                break;
        }
    }

    ui->editorTabWidget->removeTab(index);
    delete tab;
}

void MainWindow::editorTabChanged(int index)
{
    if(index < 0)
        return;
    LuaEditor* editor = static_cast<LuaEditor*>(ui->editorTabWidget->widget(index));
    if(editor)
        populateAvarView(editor->nodeRef());
}
