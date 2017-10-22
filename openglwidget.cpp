#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(std::string fileName, QWidget *parent) : QOpenGLWidget(parent), mesh(fileName),
vbo(QOpenGLBuffer::VertexBuffer), ebo(QOpenGLBuffer::IndexBuffer)
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

    ebo.create();
    ebo.bind();
    auto indices = mesh.getIndices();
    ebo.allocate(indices.data(), indices.size() * sizeof(unsigned));

    prog = std::make_unique<QOpenGLShaderProgram>(this);
    prog->addShaderFromSourceFile(QOpenGLShader::Vertex, "shader.vert");
    prog->addShaderFromSourceFile(QOpenGLShader::Fragment, "shader.frag");
    prog->link();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
}

void OpenGLWidget::resizeGL(int w, int h)
{

}

void OpenGLWidget::paintGL()
{
    prog->bind();
    vao.bind();
    glDrawElements(GL_TRIANGLES, mesh.getIndices().size(), GL_UNSIGNED_INT, 0);
}
