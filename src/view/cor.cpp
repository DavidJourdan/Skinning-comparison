#include "cor.h"

view::Cor::Cor(Core *core, QWidget *parent) : Base(core, parent)
{

}

void view::Cor::initializeGL()
{
    initializeOpenGLFunctions();
    core->initialize();

    setUpShader();
    setUpBones();
    setUpPoints();
}

void view::Cor::paintGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    draw();
    drawBones();
    drawPoints();
}

void view::Cor::setUpShader()
{
    if (!vao.create()) {
        std::cout << "Could not create vertex array object.\n";
        std::exit(EXIT_FAILURE);
    }

    vao.bind();

    core->vbo.bind();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(QVector3D));
    glEnableVertexAttribArray(1);

    core->vbo.release();

    core->corBuffer.bind();

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    core->corBuffer.release();

    core->boneDataBuffer.bind();

    for (size_t i = 0; i < MAX_BONE_COUNT / 4; ++i) {
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, MAX_BONE_COUNT * sizeof(GLfloat), reinterpret_cast<void*>(i * 4 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3 + i);
    }

    core->boneDataBuffer.release();

    core->boneIndexBuffer.bind();

    for (size_t i = 0; i < MAX_BONE_COUNT / 4; ++i) {
        glVertexAttribIPointer(3 + MAX_BONE_COUNT / 4 + i, 4, GL_UNSIGNED_INT, MAX_BONE_COUNT * sizeof(GLuint), reinterpret_cast<void*>(i * 4 * sizeof(GLuint)));
        glEnableVertexAttribArray(3 + MAX_BONE_COUNT / 4 + i);
    }

    core->boneIndexBuffer.release();

    core->boneListSizeBuffer.bind();

    glVertexAttribIPointer(3 + (2 * MAX_BONE_COUNT) / 4, 1, GL_UNSIGNED_INT, 0, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(3 + (2 * MAX_BONE_COUNT) / 4);

    core->boneListSizeBuffer.release();

    vao.release();

    prog.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/optimized_cors.vert");
    prog.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader.frag");
    prog.link();
}

void view::Cor::setUpPoints()
{
    if (!pointVao.create()) {
        std::cout << "Could not create vertex array object.\n";
        std::exit(EXIT_FAILURE);
    }

    pointVao.bind();

    core->corBuffer.bind();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    core->corBuffer.release();

    core->boneDataBuffer.bind();

    for (size_t i = 0; i < MAX_BONE_COUNT / 4; ++i) {
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, MAX_BONE_COUNT * sizeof(GLfloat), reinterpret_cast<void*>(i * 4 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1 + i);
    }

    core->boneDataBuffer.release();

    core->boneIndexBuffer.bind();

    for (size_t i = 0; i < MAX_BONE_COUNT / 4; ++i) {
        glVertexAttribIPointer(1 + MAX_BONE_COUNT / 4 + i, 4, GL_UNSIGNED_INT, MAX_BONE_COUNT * sizeof(GLuint), reinterpret_cast<void*>(i * 4 * sizeof(GLuint)));
        glEnableVertexAttribArray(1 + MAX_BONE_COUNT / 4 + i);
    }

    core->boneIndexBuffer.release();

    core->boneListSizeBuffer.bind();

    glVertexAttribIPointer(1 + (2 * MAX_BONE_COUNT) / 4, 1, GL_UNSIGNED_INT, 0, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1 + (2 * MAX_BONE_COUNT) / 4);

    core->boneListSizeBuffer.release();

    pointVao.release();

    pointProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/pointshader.vert");
    pointProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/pointshader.frag");
    pointProg.link();
}

void view::Cor::draw()
{
    glEnable(GL_DEPTH_TEST);

    prog.bind();
    vao.bind();

    prog.setUniformValue("modelMatrix", core->modelMatrix);
    prog.setUniformValue("viewMatrix", core->viewMatrix);
    prog.setUniformValue("projectionMatrix", projectionMatrix);

    const auto &quaternions = core->mesh.getSkeleton().getQuaternions();
    prog.setUniformValueArray("qArr", quaternions.data(), quaternions.size());

    const auto &transformations = core->mesh.getSkeleton().getTransformations();
    prog.setUniformValueArray("tArr", transformations.data(), transformations.size());

    core->ebo.bind();
    glPolygonMode(GL_FRONT_AND_BACK, core->meshMode);

    glDrawElements(GL_TRIANGLES, core->mesh.getTriangles().size() * 3, GL_UNSIGNED_INT, 0);
    core->ebo.release();
    vao.release();
    prog.release();
}

void view::Cor::drawPoints()
{
    if (core->showCors) {
        glDisable(GL_DEPTH_TEST);

        pointProg.bind();
        pointVao.bind();

        pointProg.setUniformValue("modelMatrix", core->modelMatrix);
        pointProg.setUniformValue("viewMatrix", core->viewMatrix);
        pointProg.setUniformValue("projectionMatrix", projectionMatrix);

        const auto &transformations = core->mesh.getSkeleton().getTransformations();
        pointProg.setUniformValueArray("tArr", transformations.data(), transformations.size());

        core->corBuffer.bind();

        glPointSize((GLfloat)5);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

        glDrawArrays(GL_POINTS, 0, core->mesh.getVertices().size());

        core->corBuffer.release();

        pointVao.release();
        pointProg.release();
    }
}
