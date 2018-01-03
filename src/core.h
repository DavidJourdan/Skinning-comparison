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

const size_t MAX_BONE_COUNT = 12;

class Core : public QOpenGLFunctions_4_1_Core
{
public:
    Core(const Config &config);

    Mesh mesh;

    // Set up OpenGL resources.
    void initialize();

    // OpenGL resources meant to be shared across multiple rendering widgets.
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

    // The model and view matrices are shared across views, as views as meant
    // to allow comparing different deforming methods in the same pose.
    QMatrix4x4 modelMatrix;
    QMatrix4x4 viewMatrix;

    void moveBone(float angle);
    void editBone(size_t i);
    void computeCoRs();
    void toggleBoneActiv();
    void resetCamera();
    void selectPreviousBone();
    void selectNextBone();
    void showBoneActiv();
    void noBoneActiv();

    GLenum meshMode;
    bool showBones { true };
    bool showCors { true };

    void updateSkeleton();

    bool initialized { false };
    bool boneSelActiv { false };
    bool isPickingBone { false };
    bool corsComputed { false };

    std::vector<OpenGLWidget*> views;
};

#endif // CORE_H
