#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QGLWidget>
#include <GL/gl.h>
#include <GL/glu.h>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

class OpenGLWidget : public QGLWidget
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget* parent)
            :QGLWidget(parent)
    {}

    void initialiseLua(lua_State* L)
    {
        m_L = L;
    }

protected:
    void initializeGL()
    {
        glClearColor(0.0,0.0,0.0,0.0);
        glEnable(GL_DEPTH_TEST);
    }

    void resizeGL(int w, int h)
    {
        glViewport(0, 0, (GLsizei)w, (GLsizei)h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        GLdouble width = w, height = h;
        if(h == 0)
            height = 1.0;
        gluPerspective(45.0, width/height, 0.1, 10000);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void paintGL();

private:
    lua_State* m_L;
};

#endif // OPENGLWIDGET_H
