#ifndef VIEW_BASE_H
#define VIEW_BASE_H

#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QPointF>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>

#include "../mesh.h"
#include "../config.h"
#include "../dualquaternion.hpp"
#include "../core.h"

namespace view {

class Base : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT

public:
    Base(Core *core, QWidget *parent);
    void updateProjMatrix();

protected:
    void initializeGL() override = 0;

    void resizeGL(int w, int h) final;

    void paintGL() override = 0;

    void mouseMoveEvent(QMouseEvent *event) final;
    void mousePressEvent(QMouseEvent *event) final;
    void mouseReleaseEvent(QMouseEvent *event) final;
    void wheelEvent(QWheelEvent *event) final;
    QVector3D viewDirection();
    QVector3D rightDirection();
    QVector3D upDirection();
    void translateCamera(QVector3D dir);

    Core *core;

    QOpenGLVertexArrayObject linevao;

    QOpenGLShaderProgram boneProg;

    QMatrix4x4 projectionMatrix;

    bool leftButtonPressed;
    bool rightButtonPressed;

    QVector3D prevPos;

    void endPickBone();

    QVector3D screenToViewport(QPointF screenPos);

    virtual void setUpShader() = 0;
    void setUpBones();

    virtual void draw() = 0;
    void drawBones();
};

}

#endif
