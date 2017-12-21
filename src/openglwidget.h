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
#include "dualquaternion.hpp"

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT

public:
    OpenGLWidget(const Config &config, QWidget *parent);

public slots:
    void editBone(size_t i);
    void moveBone(float angle);
    void deformWithLbs() { curProg = &lbsProg; }
    void deformWithDqs() { curProg = &dqsProg; }
    void deformWithOptimizedCors();
    void computeCoRs();
    void resetCamera();
    void toggleBoneActiv();
    void selectPreviousBone();
    void selectNextBone();
    void focusSelectedBone();

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
    QOpenGLVertexArrayObject pointvao;
    QOpenGLBuffer vbo;
    QOpenGLBuffer normBuffer;
    QOpenGLBuffer corBuffer;
    QOpenGLBuffer boneDataBuffer;
    QOpenGLBuffer boneIndexBuffer;
    QOpenGLBuffer boneListSizeBuffer;
    QOpenGLBuffer ebo;
    QOpenGLBuffer lineBuffer;
    QOpenGLBuffer lineIndices;
    QOpenGLBuffer lineColors;
    QOpenGLBuffer pointBuffer;
    QOpenGLBuffer pointBoneDataBuffer;
    QOpenGLBuffer pointBoneIndexBuffer;
    QOpenGLBuffer pointBoneListSizeBuffer;

    QOpenGLShaderProgram *curProg;

    QOpenGLShaderProgram lbsProg;
    QOpenGLShaderProgram optimizedCorsProg;
    QOpenGLShaderProgram dqsProg;
    QOpenGLShaderProgram boneProg;
    QOpenGLShaderProgram pointsProg;

    QMatrix4x4 modelMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projectionMatrix;

    QVector3D prevPos;

    bool leftButtonPressed;
    bool rightButtonPressed;
    bool boneSelActiv;

    QVector3D screenToViewport(QPointF screenPos);

    void updateSkeleton();
    bool corsComputed { false };

    GLenum meshMode;
};

#endif // OPENGLWIDGET_H
