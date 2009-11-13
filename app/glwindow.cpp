#include "glwindow.h"
#include "ui_glwindow.h"

GLWindow::GLWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::GLWindow)
{
    m_ui->setupUi(this);
}

GLWindow::~GLWindow()
{
    delete m_ui;
}

void GLWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
