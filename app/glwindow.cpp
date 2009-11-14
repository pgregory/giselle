#include "glwindow.h"
#include "ui_glwindow.h"

GLWindow::GLWindow(lua_State* L, QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::GLWindow),
    m_L(L)
{
    m_ui->setupUi(this);
    m_ui->graphicsView->initialiseLua(L);

    QObject::connect(m_ui->timeSlider, SIGNAL(valueChanged(int)), m_ui->graphicsView, SLOT(timeChanged(int)));
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
