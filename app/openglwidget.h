#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QGLWidget>
#include <gl/gl.h>

class OpenGLWidget : public QGLWidget
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget* parent)
            :QGLWidget(parent)
    {}

protected:
    void initializeGL()
    {
        glClearColor(0.0,0.0,0.0,0.0);
        glEnable(GL_DEPTH_TEST);
    }

    void resizeGL(int w, int h)
    {
        glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    }

    void paintGL()
    {
    }
};

#endif // OPENGLWIDGET_H
