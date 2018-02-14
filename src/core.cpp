#include "core.h"
#include "view/base.h"

#include <limits>

#include <QVector4D>

using std::numeric_limits;
using namespace std;

Core::Core(const Config &config) :
    mesh { Mesh::fromOcorFile(config.inputFile) },
    vbo(QOpenGLBuffer::VertexBuffer),
    boneDataBuffer { QOpenGLBuffer::VertexBuffer },
    boneIndexBuffer { QOpenGLBuffer::VertexBuffer },
    boneListSizeBuffer { QOpenGLBuffer::VertexBuffer },
    ebo(QOpenGLBuffer::IndexBuffer),
    lineBuffer(QOpenGLBuffer::VertexBuffer),
    lineIndices(QOpenGLBuffer::IndexBuffer),
    lineColors(QOpenGLBuffer::VertexBuffer),
    meshMode(GL_FILL)
{
    auto maxX = numeric_limits<float>::lowest();
    auto maxY = numeric_limits<float>::lowest();
    auto maxZ = numeric_limits<float>::lowest();

    auto minX = numeric_limits<float>::max();
    auto minY = numeric_limits<float>::max();
    auto minZ = numeric_limits<float>::max();

    for (auto v : mesh.getVertices()) {
        auto p = v.pos;

        if (p.x() > maxX) {
            maxX = p.x();
        }
        if (p.x() < minX) {
            minX = p.x();
        }

        if (p.y() > maxY) {
            maxY = p.y();
        }
        if (p.y() < minY) {
            minY = p.y();
        }

        if (p.z() > maxZ) {
            maxZ = p.z();
        }
        if (p.z() < minZ) {
            minZ = p.z();
        }
    }

    center = QVector3D { (minX + maxX) / 2.0f, (minY + maxY) / 2.0f, (minZ + maxZ) / 2.0f };

    auto maxDist2 = 0.0f;

    for (auto v : mesh.getVertices()) {
        auto x = (v.pos - center).lengthSquared();
        if (x > maxDist2) {
            maxDist2 = x;
        }
    }

    maxDist = sqrt(maxDist2);

    modelMatrix.translate(-center);
    viewMatrix.translate(0.0f, 0.0f, -2.0 * maxDist);
}

void Core::noBoneActiv()
{
    uint n = mesh.getSkeleton().getBones().size();
    std::vector<QVector4D> colors(2*n);
    for(uint i = 0; i < n ; i++) {
        QVector4D parentColor(1.0, 0.0, 0.0, 0.9); // black
        QVector4D childColor(1.0, 1.0, 1.0, 0.9); // red
        colors[2*i] = parentColor;
        colors[2*i + 1] = childColor;
    }
    lineColors.bind();
    lineColors.write(0, colors.data(), 2 * n * sizeof(QVector4D));
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

void Core::resetCamera()
{
    modelMatrix.setToIdentity();
    viewMatrix.setToIdentity();
    update();
}

void Core::focusSelectedBone()
{
    editBone(mesh.getBoneSelected());
    update();
}

void Core::editBone(size_t i)
{
    const auto edges = mesh.getSkeleton().getBones();

    const auto &bone = edges.at(i);
    const auto center = bone.head;
    const auto child = bone.tail;

    const auto length = (child - center).length();

    modelMatrix.setToIdentity();
    modelMatrix.translate(-center);

    const auto col = QVector4D { 0.0, 0.0, -5.0f * length, 1.0 };
    viewMatrix.setColumn(3, col);
}

void Core::updateSkeleton()
{
    const auto lines = mesh.getSkeleton().getSkelLines();
    lineBuffer.bind();
    lineBuffer.write(0, lines.data(), sizeof(QVector3D) * lines.size());
}

void Core::moveBone(float angle)
{
    QVector3D toCam(0.0, 0.0, 1.0);
    toCam = viewMatrix.inverted() * toCam;

    mesh.rotateBone(angle, toCam);
    updateSkeleton();
}

void Core::initialize()
{
    if (initialized) {
        return;
    }

    initialized = true;

    initializeOpenGLFunctions();

    vbo.create();
    vbo.bind();
    const auto& vertices = mesh.getVertices();
    vbo.allocate(vertices.data(), vertices.size() * sizeof(Vertex));

    corBuffer.create();
    corBuffer.bind();
    std::vector<QVector3D> cors = mesh.getCoRs();
    corBuffer.allocate(cors.data(), cors.size() * sizeof(QVector3D));

    auto boneData = std::vector<GLfloat>(MAX_BONE_COUNT * vertices.size());
    auto boneIndices = std::vector<GLuint>(MAX_BONE_COUNT * vertices.size());
    auto boneListSizes = std::vector<GLuint>(vertices.size());

    const auto weights = mesh.getSkeleton().getWeights();

    for (size_t i = 0; i < vertices.size(); ++i) {
        boneListSizes[i] = weights[i].size();
        for (size_t j = 0; j < weights[i].size(); ++j) {
            const auto idx = i * MAX_BONE_COUNT + j;
            boneData[idx] = weights[i][j].value;
            boneIndices[idx] = weights[i][j].boneIndex;
        }
    }

    boneDataBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    boneDataBuffer.create();
    boneDataBuffer.bind();

    boneDataBuffer.allocate(boneData.data(), sizeof(GLfloat) * vertices.size() * MAX_BONE_COUNT);

    boneIndexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    boneIndexBuffer.create();
    boneIndexBuffer.bind();

    boneIndexBuffer.allocate(boneIndices.data(), sizeof(GLuint) * vertices.size() * MAX_BONE_COUNT);

    boneListSizeBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    boneListSizeBuffer.create();
    boneListSizeBuffer.bind();

    boneListSizeBuffer.allocate(boneListSizes.data(), sizeof(GLuint) * vertices.size());

    ebo.create();
    ebo.bind();
    const auto& triangles = mesh.getTriangles();
    ebo.allocate(triangles.data(), triangles.size() * sizeof(Triangle));
    ebo.release();

    lineBuffer.create();
    lineBuffer.bind();
    std::vector<QVector3D> lines = mesh.getSkeleton().getSkelLines();
    lineBuffer.allocate(lines.data(), lines.size() * sizeof(QVector3D));

    lineColors.create();
    lineColors.bind();
    std::vector<QVector4D> colors(lines.size());
    uint n = mesh.getSkeleton().getBones().size();

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
    lineIndices.create();
    lineIndices.bind();
    std::vector<uint> ind(lines.size());
    for(uint i = 0; i < lines.size(); i++) {
        ind[i] = i;
    }
    lineIndices.allocate(ind.data(), ind.size() * sizeof(uint));
    lineIndices.release();
}

void Core::update()
{
    lbsView->update();
    dqsView->update();
    corView->update();
}

void Core::zoom(float f)
{
    zoomFactor *= f;
    lbsView->updateProjMatrix();
    dqsView->updateProjMatrix();
    corView->updateProjMatrix();
    update();
}
