#ifndef CORE_H
#define CORE_H

#include <vector>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QPointF>
#include <QOpenGLFunctions_4_1_Core>
#include <QGuiApplication>
#include <QCursor>

#include "config.h"
#include "mesh.h"

class OpenGLWidget;

class Core : public QOpenGLFunctions_4_1_Core
{
public:
    Core(const Config &config);
    void initialize();
    void moveBone(float angle);
    void editBone(size_t i);

    void computeCoRs();
    void toggleBoneActiv();
    void resetCamera();
    void selectPreviousBone();
    void selectNextBone();
    void showBoneActiv();
    void noBoneActiv();
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

    QMatrix4x4 modelMatrix;
    QMatrix4x4 viewMatrix;

    bool corsComputed { false };
    GLenum meshMode;
    bool showBones { true };
    bool showCors { true };
    bool isPickingBone { false };
    bool boneSelActiv { false };

    bool initialized;

    void updateSkeleton();

    std::vector<OpenGLWidget*> views;
};

#endif // CORE_H
