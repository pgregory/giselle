#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QtGui/QMainWindow>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

namespace Ui {
    class GLWindow;
}

class GLWindow : public QMainWindow {
    Q_OBJECT
public:
    GLWindow(lua_State* L, QWidget *parent = 0);
    ~GLWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GLWindow *m_ui;
    lua_State*  m_L;
};

#endif // GLWINDOW_H
