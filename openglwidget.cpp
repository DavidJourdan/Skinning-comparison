#include "openglwidget.h"

#include <QVector3D>

OpenGLWidget::OpenGLWidget(std::string fileName, QWidget *parent) : QOpenGLWidget(parent), window(parent), mesh(fileName),
vbo(QOpenGLBuffer::VertexBuffer), ebo(QOpenGLBuffer::IndexBuffer)
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
    auto vertices = mesh.getVertices();
    vbo.allocate(vertices.data(), vertices.size() * sizeof(Vertex));

    ebo.create();
    ebo.bind();
    auto indices = mesh.getIndices();
    ebo.allocate(indices.data(), indices.size() * sizeof(unsigned));

    prog = std::make_unique<QOpenGLShaderProgram>(this);
    prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shader.vert");
    prog->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader.frag");
    prog->link();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
}

void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-30.0, 30.0, -30.0, 30.0, -30.0, 30.0);

        GLdouble aspect = w / (h ? h : 1);

        const GLdouble zNear = -30.0, zFar = 100.0, fov = 30.0;

        perspective(fov, aspect, zNear, zFar);

        glMatrixMode(GL_MODELVIEW);
}

void OpenGLWidget::paintGL()
{
    prog->bind();
    vao.bind();

    prog->setUniformValue("viewMatrix", viewMatrix);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawElements(GL_TRIANGLES, mesh.getIndices().size(), GL_UNSIGNED_INT, 0);
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
//    qDebug() << prevPos;
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

    update();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    prevPos = screenToViewport(event->localPos());
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Escape:
            window->close();
        break;

    default:
        break;
    }
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    /*QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        scrollWithPixels(numPixels);
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        scrollWithDegrees(numSteps);
    }*/

    QPoint numDegrees = event->angleDelta() / 8;
    qreal direction = numDegrees.y() / qFabs(numDegrees.y());

    QVector3D translation(0, 0, direction);
    translation = viewMatrix.inverted() * translation;

    viewMatrix.translate(translation);

    update();
}

void OpenGLWidget::perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    GLdouble xmin, xmax, ymin, ymax;

    ymax = zNear * tan( fovy * M_PI / 360.0 );
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

QPointF OpenGLWidget::screenToViewport(QPointF screenPos)
{
    auto widthF = static_cast<float>(width());
    auto heightF = static_cast<float>(height());

    auto x = -1.0f + static_cast<float>(screenPos.x()) / widthF;
    auto y = 1.0f - static_cast<float>(screenPos.y()) / heightF;

    return QPointF(x, y);
}
