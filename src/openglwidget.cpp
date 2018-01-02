#include "openglwidget.h"

#include <QVector3D>
#include <math.h>
#include <QMessageBox>
#include <QtMath>

OpenGLWidget::OpenGLWidget(Core *core, const QString &shaderName, const bool isCorView, QWidget *parent) : QOpenGLWidget(parent),
    core { core },
    shaderName { shaderName },
    isCorView { isCorView },
    window(parent),
    prog { this },
    boneProg { this },
    pointsProg { this },
    leftButtonPressed { false },
    rightButtonPressed { false }
{
    setFocusPolicy(Qt::StrongFocus);
    core->views.push_back(this);
}

void OpenGLWidget::editBone(size_t i)
{
    core->editBone(i);
    upd();
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    core->initialize();

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    prog.addShaderFromSourceFile(QOpenGLShader::Vertex, shaderName);
    prog.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader.frag");
    prog.link();

    boneProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/boneshader.vert");
    boneProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/boneshader.frag");
    boneProg.link();

    pointsProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/pointshader.vert");
    pointsProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/pointshader.frag");
    pointsProg.link();
}

void OpenGLWidget::resizeGL(int w, int h)
{
    QMatrix4x4 mat { };
    mat.perspective(30.0f, w / static_cast<float>(h), 1.0f, 30.0f);
    projectionMatrix = mat;
}

void OpenGLWidget::paintGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    prog.bind();
    core->vao.bind();

    prog.setUniformValue("modelMatrix", core->modelMatrix);
    prog.setUniformValue("viewMatrix", core->viewMatrix);
    prog.setUniformValue("projectionMatrix", projectionMatrix);

    const auto &transformations = core->mesh.getTransformations();
    const std::vector<QVector4D> &quaternions = core->mesh.getQuaternions();
    prog.setUniformValueArray("tArr", transformations.data(), transformations.size());
    prog.setUniformValueArray("qArr", quaternions.data(), quaternions.size());

    const auto& dualPart = core->mesh.getDQuatTransformationsDualPart();
    const auto &nonDualPart = core->mesh.getDQuatTransformationsNonDualPart();
    prog.setUniformValueArray("dqTrDual", dualPart.data(), dualPart.size());
    prog.setUniformValueArray("dqTrNonDual", nonDualPart.data(), nonDualPart.size());

    core->ebo.bind();
    glPolygonMode(GL_FRONT_AND_BACK, core->meshMode);

    glDrawElements(GL_TRIANGLES, core->mesh.getIndices().size(), GL_UNSIGNED_INT, 0);
    core->ebo.release();
    core->vao.release();
    prog.release();

    glDisable(GL_DEPTH_TEST);

    auto &mesh = core->mesh;

    if (core->showBones) {
        boneProg.bind();
        core->linevao.bind();

        boneProg.setUniformValue("modelMatrix", core->modelMatrix);
        boneProg.setUniformValue("viewMatrix", core->viewMatrix);
        boneProg.setUniformValue("projectionMatrix", projectionMatrix);

        core->lineIndices.bind();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth((GLfloat)5);
        glDrawElements(GL_LINES, mesh.getSkelLines().size(), GL_UNSIGNED_INT, 0);
        core->lineIndices.release();
        core->linevao.release();
        boneProg.release();
    }

    if (core->corsComputed && core->showCors && isCorView) {
        pointsProg.bind();
        core->pointvao.bind();

        pointsProg.setUniformValue("modelMatrix", core->modelMatrix);
        pointsProg.setUniformValue("viewMatrix", core->viewMatrix);
        pointsProg.setUniformValue("projectionMatrix", projectionMatrix);

        const auto& transformations = core->mesh.getTransformations();
        pointsProg.setUniformValueArray("tArr", transformations.data(), transformations.size());

        core->pointBuffer.bind();
        glPointSize((GLfloat)5);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glDrawArrays(GL_POINTS, 0, mesh.getVertices().size());
        core->pointBuffer.release();
        core->pointvao.release();
        boneProg.release();
    }
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
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

    upd();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
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

    if (core->isPickingBone && event->button() == Qt::LeftButton && core->boneSelActiv) {
        endPickBone();
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    leftButtonPressed = false;
    rightButtonPressed = false;

    const auto cursor = QCursor { Qt::CursorShape::ArrowCursor };
    QGuiApplication::setOverrideCursor(cursor);
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta();

    if(qFabs(numDegrees.y()) != 0)
    {
        qreal zoom = numDegrees.y() / qFabs(numDegrees.y());

        core->viewMatrix.translate(zoom * viewDirection());
        upd();
    }
}

QMatrix4x4 OpenGLWidget::perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    QMatrix4x4 m;
    const float deg2rad = M_PI/180.0;
    float fovyRad = deg2rad*fovy;
    float tanHalfFovy = tan (fovyRad/2.0);

    m(0,0) = 1.0 / (aspect * tanHalfFovy);
    m(1,1) = 1.0 / tanHalfFovy;
    m(2,2) = -(zFar + zNear) / (zFar - zNear);
    m(2,3) = -1.0;
    m(3,2) = -2.0*(zFar * zNear) / (zFar - zNear);
    return m;
}

QVector3D OpenGLWidget::screenToViewport(QPointF screenPos)
{
    auto widthF = static_cast<float>(width());
    auto heightF = static_cast<float>(height());

    auto x = -1.0f + 2.0 * static_cast<float>(screenPos.x()) / widthF;
    auto y = 1.0f - 2.0 * static_cast<float>(screenPos.y()) / heightF;

    return QVector3D { QPointF { x, y } };
}

void OpenGLWidget::upd()
{
    for (auto& view : core->views) {
        view->update();
    }
}

QVector3D OpenGLWidget::viewDirection()
{
    auto &viewMatrix = core->viewMatrix;
    return QVector3D(viewMatrix(2,0), viewMatrix(2,1), viewMatrix(2,2)).normalized();
}

QVector3D OpenGLWidget::rightDirection()
{
    auto &viewMatrix = core->viewMatrix;
    return -QVector3D(viewMatrix(0,0), viewMatrix(0,1), viewMatrix(0,2)).normalized();
}

QVector3D OpenGLWidget::upDirection()
{
    auto &viewMatrix = core->viewMatrix;
    return QVector3D(viewMatrix(1,0), viewMatrix(1,1), viewMatrix(1,2)).normalized();
}

void OpenGLWidget::translateCamera(QVector3D dir)
{
    float cameraSpeed = 1.0f;
    core->viewMatrix.translate(cameraSpeed * dir);
}

void OpenGLWidget::showBoneActiv()
{
    core->showBoneActiv();
}

void OpenGLWidget::noBoneActiv()
{
    core->noBoneActiv();
}

void OpenGLWidget::endPickBone()
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
    noBoneActiv();
    showBoneActiv();
    upd();
}

void OpenGLWidget::computeCoRs() {
    core->computeCoRs();
}

void OpenGLWidget::resetCamera()
{
    core->resetCamera();
    upd();
}

void OpenGLWidget::toggleBoneActiv()
{
    core->toggleBoneActiv();
    upd();
}

void OpenGLWidget::selectPreviousBone()
{
    core->selectPreviousBone();
    upd();
}

void OpenGLWidget::selectNextBone()
{
    core->selectNextBone();
    upd();
}

void OpenGLWidget::focusSelectedBone()
{
    if (core->boneSelActiv) {
        core->editBone(core->mesh.getBoneSelected());
    }
}

void OpenGLWidget::toggleMeshMode()
{
    auto &meshMode = core->meshMode;
    meshMode = (meshMode == GL_FILL) ? GL_LINE : GL_FILL;
    upd();
}

void OpenGLWidget::toggleBoneDisplay()
{
    auto &showBones = core->showBones;
    showBones = !showBones;
    upd();
}

void OpenGLWidget::toggleCorDisplay()
{
    auto &showCors = core->showCors;
    showCors = !showCors;
    upd();
}

void OpenGLWidget::pickBone()
{
    if (core->boneSelActiv) {
        core->isPickingBone = true;

        const auto cursor = QCursor { Qt::CursorShape::CrossCursor };

        QGuiApplication::setOverrideCursor(cursor);
    }
}
