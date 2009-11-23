#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QSplitter>
#include <QtGui/QTextEdit>
#include <QtCore/QProcess>
#include <QModelIndex>

#include "luahighlighter.h"

extern "C" {
#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
}

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(lua_State *L, QWidget *parent = 0);
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

private:
    void populateTree();

    Ui::MainWindow *ui;
    QSplitter* splitter;
    QTextEdit* mainEditor;
    LuaHighlighter* highlighter;
    lua_State* L;
    QString currentType;
    QString currentName;
    int     m_currentObjectRef;
};

#endif // MAINWINDOW_H
