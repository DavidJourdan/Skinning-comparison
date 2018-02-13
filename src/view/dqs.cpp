#include "dqs.h"

view::Dqs::Dqs(Core *core, QWidget *parent) : Base(core, parent)
{

}

void view::Dqs::initializeGL()
{
    initializeOpenGLFunctions();
    core->initialize();

    setUpShader();
    setUpBones();
}

void view::Dqs::paintGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    draw();
    drawBones();
}

void view::Dqs::setUpShader()
{
    if (!vao.create()) {
        std::cout << "Could not create vertex array object.\n";
        std::exit(EXIT_FAILURE);
    }

    vao.bind();

    core->vbo.bind();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    core->vbo.release();

    core->normBuffer.bind();

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    core->normBuffer.release();

    core->boneDataBuffer.bind();

    for (size_t i = 0; i < MAX_BONE_COUNT / 4; ++i) {
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, MAX_BONE_COUNT * sizeof(GLfloat), reinterpret_cast<void*>(i * 4 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2 + i);
    }

    core->boneDataBuffer.release();

    core->boneIndexBuffer.bind();

    for (size_t i = 0; i < MAX_BONE_COUNT / 4; ++i) {
        glVertexAttribIPointer(2 + MAX_BONE_COUNT / 4 + i, 4, GL_UNSIGNED_INT, MAX_BONE_COUNT * sizeof(GLuint), reinterpret_cast<void*>(i * 4 * sizeof(GLuint)));
        glEnableVertexAttribArray(2 + MAX_BONE_COUNT / 4 + i);
    }

    core->boneIndexBuffer.release();

    core->boneListSizeBuffer.bind();

    glVertexAttribIPointer(2 + (2 * MAX_BONE_COUNT) / 4, 1, GL_UNSIGNED_INT, 0, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(2 + (2 * MAX_BONE_COUNT) / 4);

    core->boneListSizeBuffer.release();

    vao.release();

    prog.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/dqshader.vert");
    prog.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader.frag");
    prog.link();
}

void view::Dqs::draw()
{
    glEnable(GL_DEPTH_TEST);

    prog.bind();
    vao.bind();

    prog.setUniformValue("modelMatrix", core->modelMatrix);
    prog.setUniformValue("viewMatrix", core->viewMatrix);
    prog.setUniformValue("projectionMatrix", projectionMatrix);

    const auto &dualPart = core->mesh.getSkeleton().getDQuatTransformationsDualPart();
    const auto &nonDualPart = core->mesh.getSkeleton().getDQuatTransformationsNonDualPart();
    prog.setUniformValueArray("dqTrDual", dualPart.data(), dualPart.size());
    prog.setUniformValueArray("dqTrNonDual", nonDualPart.data(), nonDualPart.size());

    core->ebo.bind();
    glPolygonMode(GL_FRONT_AND_BACK, core->meshMode);

    glDrawElements(GL_TRIANGLES, core->mesh.getTriangles().size() * 3, GL_UNSIGNED_INT, 0);
    core->ebo.release();
    vao.release();
    prog.release();
}
