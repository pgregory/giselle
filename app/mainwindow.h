#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QSplitter>
#include <QtCore/QProcess>

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
};

#endif // MAINWINDOW_H
