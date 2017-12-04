#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <memory>

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QPointF>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>

#include "mesh.h"
#include "config.h"

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT

public:
    OpenGLWidget(const Config &config, QWidget *parent);

public slots:
    void editBone(size_t i);

protected:
    void initializeGL();

    void resizeGL(int w, int h);

    void paintGL();

    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    QMatrix4x4 perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
    QVector3D viewDirection();
    QVector3D rightDirection();
    QVector3D upDirection();
    void translateCamera(QVector3D dir);
    void showBoneActiv();
    void noBoneActiv();


private:
    QWidget * window;

    Mesh mesh;
    QOpenGLVertexArrayObject vao;
    QOpenGLVertexArrayObject linevao;
    QOpenGLBuffer vbo;
    QOpenGLBuffer normBuffer;
    QOpenGLBuffer ebo;
    QOpenGLBuffer lineBuffer;
    QOpenGLBuffer lineIndices;
    QOpenGLBuffer lineColors;

    std::unique_ptr<QOpenGLShaderProgram> prog;
    std::unique_ptr<QOpenGLShaderProgram> boneProg;

    QMatrix4x4 modelMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projectionMatrix;

    QPointF prevPos;

    bool leftButtonPressed;
    bool rightButtonPressed;
    bool boneSelActiv;

    QPointF screenToViewport(QPointF screenPos);
};

#endif // OPENGLWIDGET_H
