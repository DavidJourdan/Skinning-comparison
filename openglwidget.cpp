#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(std::string fileName, QWidget *parent) : QOpenGLWidget(parent), mesh(fileName)
{

}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.4, 0.6, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT);

    vao.create();
    if (vao.isCreated()) {
        vao.bind();
    }

    vbo.create();
    vbo.bind();
    auto vertices = mesh.getVertices();
    vbo.allocate(vertices.data(), vertices.size() * sizeof(glm::vec3));
}

void OpenGLWidget::resizeGL(int w, int h)
{

}

void OpenGLWidget::paintGL()
{

}
