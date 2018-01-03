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

namespace view {
class Base;
}

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

    // The model and view matrices are shared across views, as views are meant
    // to allow comparing different deforming methods in the same pose.
    QMatrix4x4 modelMatrix;
    QMatrix4x4 viewMatrix;

    // Every view applies a specific deformation method on the model.
    view::Base *lbsView;
    view::Base *dqsView;

    // Update views.
    void update();

    void moveBone(float angle);
    void editBone(size_t i);
    void computeCoRs();
    void toggleBoneActiv();
    void resetCamera();
    void focusSelectedBone();
    void showBoneActiv();
    void noBoneActiv();

    GLenum meshMode;
    bool showBones { true };
    bool showCors { true };

    void updateSkeleton();

    bool isPickingBone { false };

private:
    bool initialized { false };
    bool corsComputed { false };
};

#endif // CORE_H
