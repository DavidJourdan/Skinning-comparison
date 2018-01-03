#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
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
#include "dualquaternion.hpp"
#include "core.h"

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT

public:
    OpenGLWidget(Core *core, const QString &shaderName, const bool isCorView, QWidget *parent);

public slots:
    void editBone(size_t i);
    void computeCoRs();
    void resetCamera();
    void toggleBoneActiv();
    void selectPreviousBone();
    void selectNextBone();
    void focusSelectedBone();
    void toggleMeshMode();
    void toggleBoneDisplay();
    void toggleCorDisplay();
    void pickBone();

protected:
    void initializeGL();

    void resizeGL(int w, int h);

    void paintGL();

    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
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
    Core *core;
    const bool isCorView;

    QOpenGLVertexArrayObject vao;
    QOpenGLVertexArrayObject linevao;
    QOpenGLVertexArrayObject pointvao;

    QOpenGLShaderProgram prog;
    const QString shaderName;

    QOpenGLShaderProgram boneProg;
    QOpenGLShaderProgram pointsProg;

    QMatrix4x4 projectionMatrix;

    bool leftButtonPressed;
    bool rightButtonPressed;

    QVector3D prevPos;

    void endPickBone();

    QVector3D screenToViewport(QPointF screenPos);
    void upd();
};

#endif // OPENGLWIDGET_H
