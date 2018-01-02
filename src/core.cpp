#include "core.h"

Core::Core(const Config &config) :
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
    boneSelActiv(false),
    meshMode(GL_FILL)
{

}

void Core::noBoneActiv()
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

void Core::showBoneActiv()
{
    uint i = mesh.getBoneSelected();
    std::vector<QVector4D> d;
    d.push_back(QVector4D(0.0, 1.0, 0.0, 0.9));
    d.push_back(QVector4D(0.0, 1.0, 0.0, 0.9));
    lineColors.bind();
    lineColors.write(2*i * sizeof(QVector4D), d.data(), 2*sizeof(QVector4D));
    lineColors.release();
}

void Core::selectNextBone()
{
    if (boneSelActiv) {
        mesh.setBoneSelected(mesh.getBoneSelected()+1);
        noBoneActiv();
        showBoneActiv();
    }
}

void Core::selectPreviousBone()
{
    if (boneSelActiv) {
        mesh.setBoneSelected(mesh.getBoneSelected()-1);
        noBoneActiv();
        showBoneActiv();
    }
}

void Core::resetCamera()
{
    modelMatrix.setToIdentity();
    viewMatrix.setToIdentity();
    viewMatrix.translate(0.0f, 0.0f, -15.0f);
}

void Core::toggleBoneActiv()
{
    boneSelActiv = !boneSelActiv;
    if(boneSelActiv)
        showBoneActiv();
    else
        noBoneActiv();
}

void Core::computeCoRs() {
    if (corsComputed) {
        return;
    }
    {
        const auto cursor = QCursor { Qt::CursorShape::WaitCursor };
        QGuiApplication::setOverrideCursor(cursor);
    }

    std::vector<QVector3D> centers = mesh.computeCoRs();
    pointBuffer.bind();
    pointBuffer.write(0, centers.data(), centers.size()*sizeof(QVector3D));
    pointBuffer.release();

    corBuffer.bind();
    corBuffer.write(0, centers.data(), centers.size()*sizeof(QVector3D));
    corBuffer.release();

    corsComputed = true;

    const auto cursor = QCursor { Qt::CursorShape::ArrowCursor };
    QGuiApplication::setOverrideCursor(cursor);
}

void Core::editBone(size_t i)
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
}

void Core::updateSkeleton()
{
    const auto lines = mesh.getSkelLines();
    lineBuffer.bind();
    lineBuffer.write(0, lines.data(), sizeof(QVector3D) * lines.size());
}

void Core::moveBone(float angle)
{
    if (boneSelActiv) {
        QVector3D toCam(0.0, 0.0, 1.0);
        toCam = viewMatrix.inverted() * toCam;

        mesh.rotateBone(angle, toCam);
        updateSkeleton();
    }
}

void Core::initialize()
{
    if (initialized) {
        return;
    }

    initialized = true;

    initializeOpenGLFunctions();

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

    viewMatrix.translate(0.0f, 0.0f, -15.0f);
}
