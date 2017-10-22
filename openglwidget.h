#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include "mesh.h"

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    OpenGLWidget(std::string fileName, QWidget *parent);

protected:
    void initializeGL();

    void resizeGL(int w, int h);

    void paintGL();

private:
    Mesh mesh;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
};

#endif // OPENGLWIDGET_H
