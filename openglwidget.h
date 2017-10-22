#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <memory>

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QPointF>

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

    virtual void mouseMoveEvent(QMouseEvent *event);

private:
    Mesh mesh;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    QOpenGLBuffer ebo;

    std::unique_ptr<QOpenGLShaderProgram> prog;

    QMatrix4x4 viewMatrix;

    QPointF posWhenPressed;
};

#endif // OPENGLWIDGET_H
