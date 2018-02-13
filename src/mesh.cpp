#include "mesh.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <QDebug>

using std::array;
using std::pair;

Mesh Mesh::fromOcorFile(const string &fileName)
{
    ifstream file { fileName, std::ios::binary | std::ios::in };

    if (!file.is_open()) {
        std::cerr << "Could not open file.\n";
        std::exit(EXIT_FAILURE);
    }

    array<char, 4> id;
    file.read(id.data(), 4);
    const auto ok = id[0] == 'O' && id[1] == 'C' && id[2] == 'O' && id[3] == 'R';
    if (!ok) {
        std::cerr << "File identifier does not match.\n";
        std::exit(EXIT_FAILURE);
    }

    uint32_t endianness;
    file.read(reinterpret_cast<char *>(&endianness), 4);
    if (endianness != 0) {
        std::cerr << "Big-endian files unsupported for now\n";
    }

    uint32_t vertexCount;
    file.read(reinterpret_cast<char *>(&vertexCount), 4);

    using std::vector;
    vector<Vertex> vertices(vertexCount);
    file.read(reinterpret_cast<char *>(vertices.data()), vertices.size() * sizeof(Vertex));

    uint32_t triangleCount;
    file.read(reinterpret_cast<char *>(&triangleCount), 4);

    vector<Triangle> triangles(triangleCount);
    file.read(reinterpret_cast<char *>(triangles.data()), triangleCount * sizeof(Triangle));

    uint32_t boneCount;
    file.read(reinterpret_cast<char *>(&boneCount), 4);

    vector<Bone> boneList;
    boneList.reserve(boneCount);

    for (uint32_t i = 0; i < boneCount; ++i) {
        QVector3D head;
        file.read(reinterpret_cast<char *>(&head), 12);
        QVector3D tail;
        file.read(reinterpret_cast<char *>(&tail), 12);
        uint32_t childCount;
        file.read(reinterpret_cast<char *>(&childCount), 4);
        vector<uint32_t> childList(childCount);
        file.read(reinterpret_cast<char *>(childList.data()), 4 * childCount);

        Bone bone { head, tail, childList };
        boneList.push_back(bone);
    }

    vector<vector<Weight>> weightLists;
    weightLists.reserve(vertexCount);

    for (uint32_t i = 0; i < vertexCount; ++i) {
        uint32_t weightCount;
        file.read(reinterpret_cast<char *>(&weightCount), 4);

        vector<Weight> list(weightCount);
        file.read(reinterpret_cast<char *>(list.data()), weightCount * sizeof(Weight));

        weightLists.push_back(std::move(list));
    }

    using std::move;

    const Skeleton skeleton(move(bones),
                            move(weightLists));

    vector<QVector3D> cors(vertexCount);
    file.read(reinterpret_cast<char *>(cors.data()), cors.size() * sizeof(QVector3D));

    const Mesh retVal(move(vertices),
                      move(triangles),
                      move(skeleton),
                      move(cors));

    return retVal;
}

void Mesh::rotateBone(float angle, QVector3D axis)
{
    skeleton.rotateBone(boneSelected, angle, axis);
}
