#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QtGui/QMainWindow>

namespace Ui {
    class GLWindow;
}

class GLWindow : public QMainWindow {
    Q_OBJECT
public:
    GLWindow(QWidget *parent = 0);
    ~GLWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GLWindow *m_ui;
};

#endif // GLWINDOW_H
