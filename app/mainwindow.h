#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QSplitter>
#include <QtGui/QTextEdit>
#include <QtCore/QProcess>
#include <QModelIndex>

#include "luahighlighter.h"
#include "luaerror.h"

extern "C" {
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
}

namespace Ui
{
    class MainWindow;
}

class AvarListModel;
class SceneTreeModel;
class LuaEditor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void selectModel(const QModelIndex& index);
    void acceptChanges();
    void doRender();
    void avarsChanged(const QModelIndex& a, const QModelIndex& b);
    void startFrameChanged(int);
    void endFrameChanged(int);
    void runCommand();
    void save();
    void load();
    void editorTabClosed(int index);
    void editorTabChanged(int index);

    void writeSettings();
    void readSettings();

    void saveCode(LuaEditor* editor) throw(LuaError);

protected:
    void closeEvent(QCloseEvent*);

private:
    void populateTree(bool clear = false);
    void populateAvarView(int nodeRef);

    Ui::MainWindow *ui;
    QSplitter* splitter;
    QString currentType;
    QString currentName;
    AvarListModel* m_currentAvarModel;
    SceneTreeModel* m_sceneModel;
};

#endif // MAINWINDOW_H
