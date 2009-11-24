#include "glwindow.h"
#include "ui_glwindow.h"

#include "luaerror.h"

GLWindow::GLWindow(lua_State* L, QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::GLWindow),
    m_L(L)
{
    m_ui->setupUi(this);
    m_ui->graphicsView->initialiseLua(L);

    QObject::connect(m_ui->timeSlider, SIGNAL(valueChanged(int)), m_ui->graphicsView, SLOT(timeChanged(int)));
    QObject::connect(m_ui->actionWireframe, SIGNAL(triggered()), this, SLOT(setModeWire()));
    QObject::connect(m_ui->actionSolid, SIGNAL(triggered()), this, SLOT(setModeSolid()));
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

void GLWindow::setModeWire()
{
    try
    {
        struct C
        {
            static int call(lua_State* L)
            {
                //C* p = static_cast<C*>(lua_touserdata(L, 1));
                lua_getglobal(L, "GLRenderer");
                lua_pushstring(L, "LINES");
                lua_setfield(L, -2, "mode");
                lua_pop(L, 1);  /* << theGLRenderer" */
                return 0;
            }
        } p;
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));

    }

    catch(std::exception & e)
    {
        std::cerr << e.what() <<std::endl;
    }
}

void GLWindow::setModeSolid()
{
    try
    {
        struct C
        {
            static int call(lua_State* L)
            {
                //C* p = static_cast<C*>(lua_touserdata(L, 1));
                lua_getglobal(L, "GLRenderer");
                lua_pushstring(L, "SOLID");
                lua_setfield(L, -2, "mode");
                lua_pop(L, 1);  /* << theGLRenderer" */
                return 0;
            }
        } p;
        int res = lua_cpcall(m_L, C::call, &p);
        if(res != 0)
            throw(LuaError(m_L));

    }

    catch(std::exception & e)
    {
        std::cerr << e.what() <<std::endl;
    }
}
