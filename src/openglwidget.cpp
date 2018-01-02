#include "openglwidget.h"

#include <QVector3D>
#include <math.h>
#include <QMessageBox>
#include <QtMath>

OpenGLWidget::OpenGLWidget(const Config &config, QWidget *parent) : QOpenGLWidget(parent),
    window(parent),
    mesh { Mesh::fromCustomFile(config) },
    vbo(QOpenGLBuffer::VertexBuffer),
    normBuffer(QOpenGLBuffer::VertexBuffer),
    boneDataBuffer { QOpenGLBuffer::VertexBuffer },
    boneIndexBuffer { QOpenGLBuffer::VertexBuffer },
    boneListSizeBuffer { QOpenGLBuffer::VertexBuffer },
    ebo(QOpenGLBuffer::IndexBuffer),
    lineBuffer(QOpenGLBuffer::VertexBuffer),
    lineIndices(QOpenGLBuffer::IndexBuffer),
    lineColors(QOpenGLBuffer::VertexBuffer),
    pointBuffer(QOpenGLBuffer::VertexBuffer),
    pointBoneDataBuffer(QOpenGLBuffer::VertexBuffer),
    pointBoneIndexBuffer(QOpenGLBuffer::VertexBuffer),
    pointBoneListSizeBuffer(QOpenGLBuffer::VertexBuffer),
    curProg { nullptr },
    lbsProg { this },
    optimizedCorsProg { this },
    dqsProg { this },
    boneProg { this },
    pointsProg { this },
    leftButtonPressed(false),
    rightButtonPressed(false),
    boneSelActiv(false),
    meshMode(GL_FILL)
{
    setFocusPolicy(Qt::StrongFocus);
}

void OpenGLWidget::editBone(size_t i)
{
    const auto articulations = mesh.getArticulations();
    const auto edges = mesh.getBones();

    const auto &bone = edges.at(i);
    const auto &center = articulations.at(bone.parent);
    const auto &child = articulations.at(bone.child);

    const auto length = (child - center).length();

    modelMatrix.setToIdentity();
    modelMatrix.translate(-center);

    const auto col = QVector4D { 0.0, 0.0, -5.0f * length, 1.0 };
    viewMatrix.setColumn(3, col);

    update();
}

void OpenGLWidget::moveBone(float angle)
{
    if (boneSelActiv) {
        QVector3D toCam(0.0, 0.0, 1.0);
        toCam = viewMatrix.inverted() * toCam;

        mesh.rotateBone(angle, toCam);
        updateSkeleton();
        update();
    }
}

void OpenGLWidget::deformWithOptimizedCors() {

    if (!corsComputed) {
        auto ret = QMessageBox::question(this, "Calcul des centres de rotation",
                                         "La méthode de l'article requiert un calcul "
                                         "qui peut prendre un moment. Calculer maintenant ?"
                                         );
        if (ret == QMessageBox::Yes) {
            computeCoRs();
        }
    }

    curProg = &optimizedCorsProg;
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // VERTICES
    vao.create();
    if (vao.isCreated()) {
        vao.bind();
    }

    vbo.create();
    vbo.bind();
    std::vector<QVector3D>& vertices = mesh.getVertices();
    vbo.allocate(vertices.data(), vertices.size() * sizeof(QVector3D));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    normBuffer.create();
    normBuffer.bind();
    std::vector<QVector3D> normals = mesh.getNormals();
    normBuffer.allocate(normals.data(), normals.size() * sizeof(QVector3D));

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    corBuffer.create();
    corBuffer.bind();
    std::vector<QVector3D> cors = std::vector<QVector3D>(vertices.size());
    corBuffer.allocate(cors.data(), cors.size() * sizeof(QVector3D));

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    constexpr size_t MAX_BONE_COUNT = 12;
    auto boneData = std::vector<GLfloat>(MAX_BONE_COUNT * vertices.size());
    auto boneIndices = std::vector<GLuint>(MAX_BONE_COUNT * vertices.size());
    auto boneListSizes = std::vector<GLuint>(vertices.size());

    const auto weights = mesh.getWeights();
    const auto pBoneIndices = mesh.getBoneIndices();

    for (size_t i = 0; i < vertices.size(); ++i) {
        for (size_t j = 0; weights[i][j] > -0.5f; ++j) {
            const auto idx = i * MAX_BONE_COUNT + j;
            boneData[idx] = weights[i][j];
            boneIndices[idx] = pBoneIndices[i][j];
            ++boneListSizes[i];
        }
    }

    boneDataBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    boneDataBuffer.create();
    boneDataBuffer.bind();

    boneDataBuffer.allocate(boneData.data(), sizeof(GLfloat) * vertices.size() * MAX_BONE_COUNT);

    for (size_t i = 0; i < MAX_BONE_COUNT / 4; ++i) {
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, MAX_BONE_COUNT * sizeof(GLfloat), reinterpret_cast<void*>(i * 4 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3 + i);
    }

    boneIndexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    boneIndexBuffer.create();
    boneIndexBuffer.bind();

    boneIndexBuffer.allocate(boneIndices.data(), sizeof(GLuint) * vertices.size() * MAX_BONE_COUNT);

    for (size_t i = 0; i < MAX_BONE_COUNT / 4; ++i) {
        glVertexAttribIPointer(3 + MAX_BONE_COUNT / 4 + i, 4, GL_UNSIGNED_INT, MAX_BONE_COUNT * sizeof(GLuint), reinterpret_cast<void*>(i * 4 * sizeof(GLuint)));
        glEnableVertexAttribArray(3 + MAX_BONE_COUNT / 4 + i);
    }

    boneListSizeBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    boneListSizeBuffer.create();
    boneListSizeBuffer.bind();

    boneListSizeBuffer.allocate(boneListSizes.data(), sizeof(GLuint) * vertices.size());

    glVertexAttribIPointer(3 + (2 * MAX_BONE_COUNT) / 4, 1, GL_UNSIGNED_INT, 0, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(3 + (2 * MAX_BONE_COUNT) / 4);

    ebo.create();
    ebo.bind();
    std::vector<uint>& indices = mesh.getIndices();
    ebo.allocate(indices.data(), indices.size() * sizeof(uint));
    ebo.release();

    vao.release();

    // SKELETON
    linevao.create();
    if (linevao.isCreated()) {
        linevao.bind();
    }

    lineBuffer.create();
    lineBuffer.bind();
    std::vector<QVector3D> lines = mesh.getSkelLines();
    lineBuffer.allocate(lines.data(), lines.size() * sizeof(QVector3D));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    lineColors.create();
    lineColors.bind();
    std::vector<QVector4D> colors(lines.size());
    uint n = mesh.getEdgeNumber();

    for(uint i = 0; i < n ; i++) {
        QVector4D parentColor(1.0, 0.0, 0.0, 0.9); //red
        QVector4D childColor(1.0, 1.0, 1.0, 0.9); // white
        colors[2*i] = parentColor;
        colors[2*i + 1] = childColor;
    }

    for(uint i = 2*n ; i < lines.size() ; i++)
    {
        colors[i] = QVector4D(0.0, 0.0, 1.0, 0.9); // blue
    }

    lineColors.allocate(colors.data(), colors.size() * sizeof(QVector4D));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    lineIndices.create();
    lineIndices.bind();
    std::vector<uint> ind(lines.size());
    for(uint i = 0; i < lines.size(); i++) {
        ind[i] = i;
    }
    lineIndices.allocate(ind.data(), ind.size() * sizeof(uint));
    lineIndices.release();

    linevao.release();

    // CENTERS OF ROTATION
    pointvao.create();
    if (pointvao.isCreated()) {
        pointvao.bind();
    }

    pointBuffer.create();
    pointBuffer.bind();
    std::vector<QVector3D> points(vertices.size(), QVector3D(0, 0, 10));
    pointBuffer.allocate(points.data(), points.size() * sizeof(QVector3D));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);


    pointBoneDataBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    pointBoneDataBuffer.create();
    pointBoneDataBuffer.bind();

    pointBoneDataBuffer.allocate(boneData.data(), sizeof(GLfloat) * vertices.size() * MAX_BONE_COUNT);

    for (size_t i = 0; i < MAX_BONE_COUNT / 4; ++i) {
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, MAX_BONE_COUNT * sizeof(GLfloat), reinterpret_cast<void*>(i * 4 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1 + i);
    }

    pointBoneIndexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    pointBoneIndexBuffer.create();
    pointBoneIndexBuffer.bind();

    pointBoneIndexBuffer.allocate(boneIndices.data(), sizeof(GLuint) * vertices.size() * MAX_BONE_COUNT);

    for (size_t i = 0; i < MAX_BONE_COUNT / 4; ++i) {
        glVertexAttribIPointer(1 + MAX_BONE_COUNT / 4 + i, 4, GL_UNSIGNED_INT, MAX_BONE_COUNT * sizeof(GLuint), reinterpret_cast<void*>(i * 4 * sizeof(GLuint)));
        glEnableVertexAttribArray(1 + MAX_BONE_COUNT / 4 + i);
    }

    pointBoneListSizeBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    pointBoneListSizeBuffer.create();
    pointBoneListSizeBuffer.bind();

    pointBoneListSizeBuffer.allocate(boneListSizes.data(), sizeof(GLuint) * vertices.size());

    glVertexAttribIPointer(1 + (2 * MAX_BONE_COUNT) / 4, 1, GL_UNSIGNED_INT, 0, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1 + (2 * MAX_BONE_COUNT) / 4);

    lbsProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/lbs_shader.vert");
    lbsProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader.frag");
    lbsProg.link();

    dqsProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/dqshader.vert");
    dqsProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader.frag");
    dqsProg.link();

    optimizedCorsProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/optimized_cors.vert");
    optimizedCorsProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader.frag");
    optimizedCorsProg.link();

    boneProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/boneshader.vert");
    boneProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/boneshader.frag");
    boneProg.link();

    pointsProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/pointshader.vert");
    pointsProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/pointshader.frag");
    pointsProg.link();

    curProg = &lbsProg;

    viewMatrix.translate(0.0f, 0.0f, -15.0f);
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

    curProg->bind();
    vao.bind();

    curProg->setUniformValue("modelMatrix", modelMatrix);
    curProg->setUniformValue("viewMatrix", viewMatrix);
    curProg->setUniformValue("projectionMatrix", projectionMatrix);

    const auto &transformations = mesh.getTransformations();
    const std::vector<QVector4D> &quaternions = mesh.getQuaternions();
    curProg->setUniformValueArray("tArr", transformations.data(), transformations.size());
    curProg->setUniformValueArray("qArr", quaternions.data(), quaternions.size());

    const auto& dualPart = mesh.getDQuatTransformationsDualPart();
    const auto &nonDualPart = mesh.getDQuatTransformationsNonDualPart();
    curProg->setUniformValueArray("dqTrDual", dualPart.data(), dualPart.size());
    curProg->setUniformValueArray("dqTrNonDual", nonDualPart.data(), nonDualPart.size());

    ebo.bind();
    glPolygonMode(GL_FRONT_AND_BACK, meshMode);

    glDrawElements(GL_TRIANGLES, mesh.getIndices().size(), GL_UNSIGNED_INT, 0);
    ebo.release();
    vao.release();
    curProg->release();

    glDisable(GL_DEPTH_TEST);

    if (showBones) {
        boneProg.bind();
        linevao.bind();

        boneProg.setUniformValue("modelMatrix", modelMatrix);
        boneProg.setUniformValue("viewMatrix", viewMatrix);
        boneProg.setUniformValue("projectionMatrix", projectionMatrix);

        lineIndices.bind();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth((GLfloat)5);
        glDrawElements(GL_LINES, mesh.getSkelLines().size(), GL_UNSIGNED_INT, 0);
        lineIndices.release();
        linevao.release();
        boneProg.release();
    }

    if (curProg == &optimizedCorsProg && corsComputed && showCors) {
        pointsProg.bind();
        pointvao.bind();

        pointsProg.setUniformValue("modelMatrix", modelMatrix);
        pointsProg.setUniformValue("viewMatrix", viewMatrix);
        pointsProg.setUniformValue("projectionMatrix", projectionMatrix);

        const auto& transformations = mesh.getTransformations();
        pointsProg.setUniformValueArray("tArr", transformations.data(), transformations.size());

        pointBuffer.bind();
        glPointSize((GLfloat)5);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glDrawArrays(GL_POINTS, 0, mesh.getVertices().size());
        pointBuffer.release();
        pointvao.release();
        boneProg.release();
    }
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isPickingBone) {
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

        movementY = viewMatrix.inverted() * movementY;
        movementX = viewMatrix.inverted() * movementX;

        const auto yAxis = QVector3D { 0.0, 1.0, 0.0 };

        const auto rotVec0 = rightDirection();
        const auto rotVec1 = yAxis;

        viewMatrix.rotate(angle0, rotVec0);
        viewMatrix.rotate(angle1, rotVec1);

        prevPos = pos;
    } else if (leftButtonPressed) {
        const auto pos = screenToViewport(event->localPos());
        const auto movement = pos - prevPos;
        const auto center = mesh.getArticulations()[mesh.getBones()[mesh.getBoneSelected()].parent];
        const auto x = prevPos - projectionMatrix * viewMatrix * modelMatrix * center;

        const auto t0 = QVector3D::crossProduct(QVector3D { 0.0, 0.0, 1.0 }, x);

        const auto angle = qRadiansToDegrees(QVector3D::dotProduct(t0, movement) / t0.lengthSquared());

        moveBone(angle);

        prevPos = pos;
    }

    else if(rightButtonPressed)
    {
        auto pos = screenToViewport(event->localPos());

        QVector3D movement{pos - prevPos};

        movement = QVector3D(-rightDirection() * movement.x() + upDirection() * movement.y());

        float speed = 2.5f;
        viewMatrix.translate(movement * speed);

        prevPos = pos;
    }

    update();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && !rightButtonPressed)
        leftButtonPressed = true;

    if(event->button() == Qt::RightButton && !leftButtonPressed)
        rightButtonPressed = true;

    prevPos = screenToViewport(event->localPos());

    if (isPickingBone && event->button() == Qt::LeftButton && boneSelActiv) {
        isPickingBone = false;

        const auto &bones = mesh.getBones();
        const auto &art = mesh.getArticulations();

        size_t bone;
        auto min = std::numeric_limits<float>::max();

        const auto sqDst = [&](const Bone &bone) {
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

        for (size_t i = 0; i < mesh.getEdgeNumber(); ++i) {
            const auto d = sqDst(bones[i]);
            if (d < min) {
                min = d;
                bone = i;
            }
        }

        mesh.setBoneSelected(bone);
        noBoneActiv();
        showBoneActiv();
        update();
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    leftButtonPressed = false;
    rightButtonPressed = false;
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta();

    if(qFabs(numDegrees.y()) != 0)
    {
        qreal zoom = numDegrees.y() / qFabs(numDegrees.y());

        viewMatrix.translate(zoom * viewDirection());
        update();
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

void OpenGLWidget::updateSkeleton()
{
    const auto lines = mesh.getSkelLines();
    lineBuffer.bind();
    lineBuffer.write(0, lines.data(), sizeof(QVector3D) * lines.size());
}

QVector3D OpenGLWidget::viewDirection()
{
    return QVector3D(viewMatrix(2,0), viewMatrix(2,1), viewMatrix(2,2)).normalized();
}

QVector3D OpenGLWidget::rightDirection()
{
    return -QVector3D(viewMatrix(0,0), viewMatrix(0,1), viewMatrix(0,2)).normalized();
}

QVector3D OpenGLWidget::upDirection()
{
    return QVector3D(viewMatrix(1,0), viewMatrix(1,1), viewMatrix(1,2)).normalized();
}

void OpenGLWidget::translateCamera(QVector3D dir)
{
    float cameraSpeed = 1.0f;
    viewMatrix.translate(cameraSpeed * dir);
}

void OpenGLWidget::showBoneActiv()
{
    uint i = mesh.getBoneSelected();
    std::vector<QVector4D> d;
    d.push_back(QVector4D(0.0, 1.0, 0.0, 0.9));
    d.push_back(QVector4D(0.0, 1.0, 0.0, 0.9));
    lineColors.bind();
    lineColors.write(2*i * sizeof(QVector4D), d.data(), 2*sizeof(QVector4D));
    lineColors.release();
}

void OpenGLWidget::noBoneActiv()
{
    uint n = mesh.getEdgeNumber();
    std::vector<QVector4D> colors(2*n);
    for(uint i = 0; i < n ; i++) {
        QVector4D parentColor(1.0, 0.0, 0.0, 0.9); //black
        QVector4D childColor(1.0, 1.0, 1.0, 0.9); // red
        colors[2*i] = parentColor;
        colors[2*i + 1] = childColor;
    }
    lineColors.bind();
    lineColors.write(0, colors.data(), 2*n*sizeof(QVector4D));
    lineColors.release();
}

void OpenGLWidget::computeCoRs() {
    if (corsComputed) {
        return;
    }

    std::vector<QVector3D> centers = mesh.computeCoRs();
    pointBuffer.bind();
    pointBuffer.write(0, centers.data(), centers.size()*sizeof(QVector3D));
    pointBuffer.release();

    corBuffer.bind();
    corBuffer.write(0, centers.data(), centers.size()*sizeof(QVector3D));
    corBuffer.release();

    corsComputed = true;
}

void OpenGLWidget::resetCamera()
{
    modelMatrix.setToIdentity();
    viewMatrix.setToIdentity();
    viewMatrix.translate(0.0f, 0.0f, -15.0f);
    update();
}

void OpenGLWidget::toggleBoneActiv()
{
    boneSelActiv = !boneSelActiv;
    if(boneSelActiv)
        showBoneActiv();
    else
        noBoneActiv();
    update();
}

void OpenGLWidget::selectPreviousBone()
{
    if (boneSelActiv) {
        mesh.setBoneSelected(mesh.getBoneSelected()-1);
        noBoneActiv();
        showBoneActiv();
        update();
    }
}

void OpenGLWidget::selectNextBone()
{
    if (boneSelActiv) {
        mesh.setBoneSelected(mesh.getBoneSelected()+1);
        noBoneActiv();
        showBoneActiv();
        update();
    }
}

void OpenGLWidget::focusSelectedBone()
{
    if (boneSelActiv) {
        editBone(mesh.getBoneSelected());
    }
}

void OpenGLWidget::toggleMeshMode()
{
    meshMode = (meshMode == GL_FILL)? GL_LINE : GL_FILL;
    update();
}

void OpenGLWidget::toggleBoneDisplay()
{
    showBones = !showBones;
    update();
}

void OpenGLWidget::toggleCorDisplay()
{
    showCors = !showCors;
    update();
}

void OpenGLWidget::pickBone()
{
    if (boneSelActiv) {
        isPickingBone = true;
    }
}
