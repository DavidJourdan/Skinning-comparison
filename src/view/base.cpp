#include "base.h"

#include <QVector3D>
#include <math.h>
#include <QtMath>

namespace view {

Base::Base(Core *core, QWidget *parent) : QOpenGLWidget(parent),
    core { core },
    boneProg { this },
    leftButtonPressed { false },
    rightButtonPressed { false }
{
    setFocusPolicy(Qt::StrongFocus);
}

void Base::resizeGL(int w, int h)
{
    QMatrix4x4 mat { };
    mat.perspective(30.0f, w / static_cast<float>(h), 1.0f, 30.0f);
    projectionMatrix = mat;
}

void Base::mouseMoveEvent(QMouseEvent *event)
{
    if (core->isPickingBone) {
        return;
    }

    const auto mod = event->modifiers();

    if(leftButtonPressed && !(mod & Qt::ControlModifier))
    {
        auto pos = screenToViewport(event->localPos());
        QVector3D movement{ pos - prevPos };

        constexpr float rotFactor = 1e2;

        auto movementX = QVector3D(movement.x(), 0.0, 0.0);
        auto movementY = QVector3D(0.0, movement.y(), 0.0);

        const auto angle0 = rotFactor * movementY.y();
        const auto angle1 = rotFactor * movementX.x();

        movementY = core->viewMatrix.inverted() * movementY;
        movementX = core->viewMatrix.inverted() * movementX;

        const auto yAxis = QVector3D { 0.0, 1.0, 0.0 };

        const auto rotVec0 = rightDirection();
        const auto rotVec1 = yAxis;

        core->viewMatrix.rotate(angle0, rotVec0);
        core->viewMatrix.rotate(angle1, rotVec1);

        prevPos = pos;
    } else if (leftButtonPressed) {
        auto &mesh = core->mesh;

        const auto pos = screenToViewport(event->localPos());
        const auto movement = pos - prevPos;
        const auto center = mesh.getArticulations()[mesh.getBones()[mesh.getBoneSelected()].parent];
        const auto x = prevPos - projectionMatrix * core->viewMatrix * core->modelMatrix * center;

        const auto t0 = QVector3D::crossProduct(QVector3D { 0.0, 0.0, 1.0 }, x);

        const auto angle = qRadiansToDegrees(QVector3D::dotProduct(t0, movement) / t0.lengthSquared());

        core->moveBone(angle);

        prevPos = pos;
    }

    else if(rightButtonPressed)
    {
        auto pos = screenToViewport(event->localPos());

        QVector3D movement{pos - prevPos};

        movement = QVector3D(-rightDirection() * movement.x() + upDirection() * movement.y());

        float speed = 2.5f;
        core->viewMatrix.translate(movement * speed);

        prevPos = pos;
    }

    core->update();
}

void Base::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && !rightButtonPressed) {
        leftButtonPressed = true;

        const auto cursor = QCursor { Qt::CursorShape::ClosedHandCursor };
        QGuiApplication::setOverrideCursor(cursor);
    }

    if(event->button() == Qt::RightButton && !leftButtonPressed) {
        rightButtonPressed = true;

        const auto cursor = QCursor { Qt::CursorShape::ClosedHandCursor };
        QGuiApplication::setOverrideCursor(cursor);
    }

    prevPos = screenToViewport(event->localPos());

    if (core->isPickingBone && event->button() == Qt::LeftButton) {
        endPickBone();
    }
}

void Base::mouseReleaseEvent(QMouseEvent *event)
{
    leftButtonPressed = false;
    rightButtonPressed = false;

    const auto cursor = QCursor { Qt::CursorShape::ArrowCursor };
    QGuiApplication::setOverrideCursor(cursor);
}

void Base::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta();

    if(qFabs(numDegrees.y()) != 0)
    {
        qreal zoom = numDegrees.y() / qFabs(numDegrees.y());

        core->viewMatrix.translate(zoom * viewDirection());
        core->update();
    }
}

QVector3D Base::screenToViewport(QPointF screenPos)
{
    auto widthF = static_cast<float>(width());
    auto heightF = static_cast<float>(height());

    auto x = -1.0f + 2.0 * static_cast<float>(screenPos.x()) / widthF;
    auto y = 1.0f - 2.0 * static_cast<float>(screenPos.y()) / heightF;

    return QVector3D { QPointF { x, y } };
}

void Base::setUpBones()
{
    linevao.create();
    if (linevao.isCreated()) {
        linevao.bind();
    }

    core->lineBuffer.bind();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    core->lineColors.bind();

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    boneProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/boneshader.vert");
    boneProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/boneshader.frag");
    boneProg.link();
}

void Base::drawBones()
{
    glDisable(GL_DEPTH_TEST);

    auto &mesh = core->mesh;

    if (core->showBones) {
        boneProg.bind();
        linevao.bind();

        boneProg.setUniformValue("modelMatrix", core->modelMatrix);
        boneProg.setUniformValue("viewMatrix", core->viewMatrix);
        boneProg.setUniformValue("projectionMatrix", projectionMatrix);

        core->lineIndices.bind();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth((GLfloat)5);
        glDrawElements(GL_LINES, mesh.getSkelLines().size(), GL_UNSIGNED_INT, 0);
        core->lineIndices.release();
        linevao.release();
        boneProg.release();
    }
}

QVector3D Base::viewDirection()
{
    auto &viewMatrix = core->viewMatrix;
    return QVector3D(viewMatrix(2,0), viewMatrix(2,1), viewMatrix(2,2)).normalized();
}

QVector3D Base::rightDirection()
{
    auto &viewMatrix = core->viewMatrix;
    return -QVector3D(viewMatrix(0,0), viewMatrix(0,1), viewMatrix(0,2)).normalized();
}

QVector3D Base::upDirection()
{
    auto &viewMatrix = core->viewMatrix;
    return QVector3D(viewMatrix(1,0), viewMatrix(1,1), viewMatrix(1,2)).normalized();
}

void Base::translateCamera(QVector3D dir)
{
    float cameraSpeed = 1.0f;
    core->viewMatrix.translate(cameraSpeed * dir);
}

void Base::endPickBone()
{
    core->isPickingBone = false;
    const auto cursor = QCursor { Qt::CursorShape::ArrowCursor };
    QGuiApplication::setOverrideCursor(cursor);

    const auto &bones = core->mesh.getBones();
    const auto &art = core->mesh.getArticulations();

    size_t bone;
    auto min = std::numeric_limits<float>::max();

    const auto sqDst = [&](const Bone &bone) {
        auto &viewMatrix = core->viewMatrix;
        auto &modelMatrix = core->modelMatrix;

        auto p0 = projectionMatrix * viewMatrix * modelMatrix * art[bone.parent];
        auto p1 = projectionMatrix * viewMatrix * modelMatrix * art[bone.child];

        p0.setZ(0.0f);
        p1.setZ(0.0f);

        const auto d = p1 - p0;

        const auto v0 = prevPos - p0;
        const auto v1 = prevPos - p1;

        if (QVector3D::dotProduct(v0, d) < 0.0f) {
            return v0.lengthSquared();
        } else if (QVector3D::dotProduct(v1, d) > 0.0f) {
            return v1.lengthSquared();
        }

        const auto proj = QVector3D::dotProduct(v0, d) / d.lengthSquared() * d;
        return v0.lengthSquared() - proj.lengthSquared();
    };

    for (size_t i = 0; i < core->mesh.getEdgeNumber(); ++i) {
        const auto d = sqDst(bones[i]);
        if (d < min) {
            min = d;
            bone = i;
        }
    }

    core->mesh.setBoneSelected(bone);
    core->noBoneActiv();
    core->showBoneActiv();
    core->update();
}

}
