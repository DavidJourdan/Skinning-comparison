#include "openglwidget.h"

#include <QVector3D>

OpenGLWidget::OpenGLWidget(std::string fileName, QWidget *parent) : QOpenGLWidget(parent), window(parent), mesh(fileName),
    vbo(QOpenGLBuffer::VertexBuffer), normBuffer(QOpenGLBuffer::VertexBuffer), ebo(QOpenGLBuffer::IndexBuffer),
    leftButtonPressed(false), rightButtonPressed(false)
{
    setFocusPolicy(Qt::StrongFocus);
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT);

   glEnable(GL_DEPTH_TEST);

    vao.create();
    if (vao.isCreated()) {
        vao.bind();
    }

    vbo.create();
    vbo.bind();
    std::vector<Vertex>& vertices = mesh.getVertices();
    vbo.allocate(vertices.data(), vertices.size() * sizeof(Vertex));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);


    normBuffer.create();
    normBuffer.bind();
    std::vector<QVector3D> normals = mesh.getNormals();
    normBuffer.allocate(normals.data(), normals.size() * sizeof(QVector3D));

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    ebo.create();
    ebo.bind();
    std::vector<uint>& indices = mesh.getIndices();
    ebo.allocate(indices.data(), indices.size() * sizeof(uint));
    ebo.release();

    prog = std::make_unique<QOpenGLShaderProgram>(this);
    prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shader.vert");
    prog->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader.frag");
    prog->link();


    viewMatrix.translate(0.0f, 0.0f, -10.0f);
}

void OpenGLWidget::resizeGL(int w, int h)
{
        GLdouble aspect = w / (h ? h : 1);
        const GLdouble zNear = 1.0, zFar = 30.0, fov = 30.0;
        projectionMatrix = perspective(fov, aspect, zNear, zFar);
}

void OpenGLWidget::paintGL()
{
    prog->bind();
    vao.bind();

    prog->setUniformValue("modelMatrix", modelMatrix);
    prog->setUniformValue("viewMatrix", viewMatrix);
    prog->setUniformValue("projectionMatrix", projectionMatrix);

    ebo.bind();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawElements(GL_TRIANGLES, mesh.getIndices().size(), GL_UNSIGNED_INT, 0);
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{


//    qDebug() << prevPos;
    if(leftButtonPressed)
    {
        auto pos = screenToViewport(event->localPos());
        QVector3D movement{pos - prevPos};
        constexpr float rotFactor = 1e2;
        float angle = movement.length() * rotFactor;
        movement.normalize();
        QVector3D toCam{0.0f, 0.0f, 1.0f};
        movement = viewMatrix.inverted() * movement;
        toCam = viewMatrix.inverted() * toCam;
        auto rotVec = QVector3D::crossProduct(toCam, movement);

        viewMatrix.rotate(angle, rotVec);

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
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    leftButtonPressed = false;
    rightButtonPressed = false;
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
        window->close();
        break;

    case Qt::Key_R: //reset camera
        viewMatrix.setToIdentity();
        viewMatrix.translate(0.0f, 0.0f, -10.0f);
        update();
        break;

    default:
        break;
    }
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

QPointF OpenGLWidget::screenToViewport(QPointF screenPos)
{
    auto widthF = static_cast<float>(width());
    auto heightF = static_cast<float>(height());

    auto x = -1.0f + static_cast<float>(screenPos.x()) / widthF;
    auto y = 1.0f - static_cast<float>(screenPos.y()) / heightF;

    return QPointF(x, y);
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
