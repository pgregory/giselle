#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QSplitter>
#include <QtGui/QTextEdit>
#include <QtCore/QProcess>

#include "luahighlighter.h"

struct lua_State;

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

private:
    Ui::MainWindow *ui;
    QSplitter* splitter;
    QTextEdit* mainEditor;
    LuaHighlighter* highlighter;
};

#endif // MAINWINDOW_H
