#include "core.h"
#include "view/base.h"

using namespace std;

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
    corColors(QOpenGLBuffer::VertexBuffer),
    meshMode(GL_FILL),
    fileName(config.inputFile)
{

}

void Core::noBoneActiv()
{
    uint n = mesh.getEdgeNumber();
    std::vector<QVector4D> colors(2*n);
    for(uint i = 0; i < n ; i++) {
        QVector4D parentColor(1.0, 0.0, 0.0, 1.0); //red
        QVector4D childColor(1.0, 1.0, 1.0, 1.0); // black
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

void Core::noCorActiv()
{
    uint n = mesh.getCoRs().size();
    std::vector<QVector4D> colors(n, QVector4D(1.0, 0.0, 0.0, 1.0));

        corColors.bind();
        corColors.write(0, colors.data(), n * sizeof(QVector4D));
        corColors.release();

}

void Core::showCorActiv()
{
    uint i = mesh.getCorSelected();
    QVector4D data[1] = { QVector4D(0.0, 0.8, 0.8, 1.0) };
    corColors.bind();
    corColors.write(i * sizeof(QVector4D), data, sizeof(QVector4D));
    corColors.release();
}


void Core::resetCamera()
{
    modelMatrix.setToIdentity();
    viewMatrix.setToIdentity();
    viewMatrix.translate(0.0f, 0.0f, -15.0f);
    update();
}

void Core::focusSelectedBone()
{
    editBone(mesh.getBoneSelected());
    update();
}

void Core::computeCoRs() {
    if (corsComputed_) {
        return;
    }
    std::vector<QVector3D> centers;

    QString name = QString(fileName.data());
    name = name.split("/").last();
    name = "ressources/" + name;
    int size = name.size();

    name.truncate(size - 4); // remove the '.obj'
    name += ".cor";

    QFile file(name);

    if(file.exists()) {
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {

            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList list = line.split(" ");
                centers.push_back(QVector3D(list.at(0).toFloat(), list.at(1).toFloat(), list.at(2).toFloat()));
            }
        }

        mesh.setCoRs(centers);
    } else {
        {        
            const auto cursor = QCursor { Qt::CursorShape::WaitCursor };
            QGuiApplication::setOverrideCursor(cursor);
        }

        centers = mesh.computeCoRs();

        const auto cursor = QCursor { Qt::CursorShape::ArrowCursor };
        QGuiApplication::setOverrideCursor(cursor);

        if (file.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream out(&file);
            for(QVector3D v : centers) {
                out << v.x() << " " << v.y() << " " << v.z() << "\n";
            }
        }
    }

    corBuffer.bind();
    corBuffer.write(0, centers.data(), centers.size()*sizeof(QVector3D));
    corBuffer.release();

    corsComputed_ = true;
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
    std::vector<QVector3D>& vertices = mesh.getVertices();
    vbo.allocate(vertices.data(), vertices.size() * sizeof(QVector3D));

    normBuffer.create();
    normBuffer.bind();
    std::vector<QVector3D> normals = mesh.getNormals();
    normBuffer.allocate(normals.data(), normals.size() * sizeof(QVector3D));

    corBuffer.create();
    corBuffer.bind();
    std::vector<QVector3D> cors = std::vector<QVector3D>(vertices.size());
    corBuffer.allocate(cors.data(), cors.size() * sizeof(QVector3D));

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

    corColors.create();
    corColors.bind();
    std::vector<QVector4D> colorCors(vertices.size(), QVector4D(1., 0., 0., 1.));
    corColors.allocate(colorCors.data(), 4 * sizeof(GLfloat) * colorCors.size());

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
    std::vector<uint>& indices = mesh.getIndices();
    ebo.allocate(indices.data(), indices.size() * sizeof(uint));
    ebo.release();

    lineBuffer.create();
    lineBuffer.bind();
    std::vector<QVector3D> lines = mesh.getSkelLines();
    lineBuffer.allocate(lines.data(), lines.size() * sizeof(QVector3D));

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
    lineIndices.create();
    lineIndices.bind();
    std::vector<uint> ind(lines.size());
    for(uint i = 0; i < lines.size(); i++) {
        ind[i] = i;
    }
    lineIndices.allocate(ind.data(), ind.size() * sizeof(uint));
    lineIndices.release();

    viewMatrix.translate(0.0f, 0.0f, -15.0f);
}

void Core::update()
{
    lbsView->update();
    dqsView->update();
    corView->update();
}
