#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QGLWidget>
#include <QMessageBox>
#include <GL/gl.h>
#include <GL/glu.h>

#include "luaerror.h"

class OpenGLWidget : public QGLWidget
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget* parent)
            :QGLWidget(parent), m_time(0)
    {}

    void initialise();
    QSize sizeHint() const
    {
        return QSize(320,240);
    }

public slots:
    void timeChanged(int time);
    void cameraChanged(QString cameraName);

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
        gluPerspective(45.0, width/height, 1, 100);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void paintGL();

private:
    int        m_time;
    int        m_glRendererRef;
    QString    m_cameraName;
};

#endif // OPENGLWIDGET_H
