#include "mesh.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <QDebug>

using namespace std;


Mesh Mesh::fromCustomFile(const Config &config)
{
    std::ifstream meshFile { config.inputFile };

    if (!meshFile.is_open()) {
        throw std::runtime_error { "Failed to open mesh file." };
    }

    std::vector<QVector3D> vertices;
    std::vector<unsigned> indices;

    for (std::string line; std::getline(meshFile, line); ) {
        std::stringstream lineStream { line };

        std::string first;
        lineStream >> first;

        if (first == "v") {
            float x;
            float y;
            float z;

            lineStream >> x >> y >> z;
            QVector3D vertex { x, y, z };

            vertices.push_back(vertex);
        } else if (first == "f") {
            std::vector<unsigned> face;

            for (unsigned index; lineStream >> index;) {
                face.push_back(index - 1);
            }

            switch (face.size()) {
            case 3:
                for (auto index : face) {
                    indices.push_back(index);
                }

                break;
            case 4:
                for (size_t i = 0; i < 3; ++i) {
                    indices.push_back(face[i]);
                }

                indices.push_back(face[2]);
                indices.push_back(face[3]);
                indices.push_back(face[0]);

                break;
            default:
                break;
            }
        }
    }

    auto normals = std::vector<QVector3D>(vertices.size());

    for (size_t i = 0; i < indices.size(); i += 3) {
        const auto& a = vertices[indices[i]];
        const auto& b = vertices[indices[i + 1]];
        const auto& c = vertices[indices[i + 2]];

        const auto cross = QVector3D::crossProduct(b - a, c - a);
        for (size_t j = 0; j < 3; ++j) {
            normals[indices[i + j]] += cross;
        }
    }

    for (auto& normal : normals) {
        normal.normalize();
    }

    Skeleton skeleton(config.skelFile, config.weightFile, vertices.size());

    return Mesh(vertices, indices, normals, skeleton);
}

void Mesh::rotateBone(float angle, QVector3D axis)
{
    skeleton.rotateBone(boneSelected, angle, axis);
}

float Mesh::area(Triangle t) {
    QVector3D v = QVector3D::crossProduct( vertices[t.b]-vertices[t.a], vertices[t.c]-vertices[t.a]);
    return v.length()/2;
}

Mesh::Mesh(std::vector<QVector3D> vertices,
           std::vector<unsigned> indices,
           Skeleton skeleton) : vertices { vertices },
    indices { indices }, skeleton { skeleton }
{
    CoRs.reserve(indices.size());
}

Mesh::Mesh(std::vector<QVector3D> vertices,
           std::vector<unsigned> indices,
           std::vector<QVector3D> normals,
           Skeleton skeleton) : vertices { vertices },
    indices { indices }, normals { normals }, skeleton { skeleton }
{
    CoRs.reserve(indices.size());
}

const vector<QVector3D> &Mesh::computeCoRs() {
    // subdivide mesh
    // compute CoRs
    for(uint i= 0; i < vertices.size(); i++) {
        QVector3D c;
        float s = 0.0;
        uint count = indices.size()/3;
        for(uint j = 0; j < count; j++) {
            Triangle t = {indices[3*j], indices[3*j+1], indices[3*j+2]};
            c += (vertices[t.a]+vertices[t.b]+vertices[t.c])/3.*skeleton.simil(i, t)*area(t);
            s += skeleton.simil(i, t) * area(t);
        }
        if(abs(s) < 1e-10) { // vertex is (almost) controlled by a unique bone
            CoR.push_back(QVector3D(0,0,0)); // we default to the standard LBS case
        } else CoRs.push_back(c/s);
    }
    return CoRs;
}

QVector3D Mesh::computeCoR(uint i) {
    QVector3D c;
    float s = 0.0;
    uint count = indices.size()/3;
    for(uint j = 0; j < count; j++) {
        Triangle t = {indices[3*j], indices[3*j+1], indices[3*j+2]};
        float similArea = skeleton.simil(i, t)*area(t);
        
        c += (vertices[t.a]+vertices[t.b]+vertices[t.c])/3.*similArea;
        s += similArea;
        if(isnan(similArea) || isinf(similArea)) {
            cout << "simil " << skeleton.simil(i, t) << endl;
            cout << "area " << area(t) << endl;
        }
    }
    return c/((float) s);
}
