#include "openglwidget.h"
#include "datamanager.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include "luaerror.h"

void OpenGLWidget::initialise()
{
    // Now create a new GLRenderer for our use.
    try
    {
        m_glRendererRef = DataManager::instance().createGLRendererRef();
    }
    catch(std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
    }
}


void OpenGLWidget::paintGL()
{
    try
    {
        DataManager::instance().renderGL(m_glRendererRef, m_time, m_cameraName);
    }
    catch(std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        // \todo: Want to output the message to the interface here, but if
        // I do, the render loop forces us into an infinite loop, so for now it
        // just goes to stderr.
        //QMessageBox msgBox;
        //msgBox.setText(e.what());
        //msgBox.exec();
    }
}

void OpenGLWidget::timeChanged(int time)
{
    m_time = time;
    updateGL();
}

void OpenGLWidget::cameraChanged(QString cameraName)
{
    m_cameraName = cameraName;
    updateGL();
}
