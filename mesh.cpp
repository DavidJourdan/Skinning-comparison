#include "mesh.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <QDebug>

Mesh Mesh::fromGenericFile(const std::string &fileName)
{
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(fileName,
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_GenNormals);

    if (!scene) {
        std::cerr << importer.GetErrorString();

        // Maybe throw an exception here.
        std::exit(EXIT_FAILURE);
    }

    // Check that there is at least one mesh.
    if (scene->mNumMeshes == 0) {
        std::cout << "Could not load a mesh" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // For testing purpose, just trying to get one mesh here.
    const aiMesh* mesh = scene->mMeshes[0];

    std::vector<QVector3D> vertices;
    std::vector<unsigned> indices;
    std::vector<QVector3D> normals;
    Skeleton skeleton;

    vertices.reserve(mesh->mNumVertices);
    normals.reserve(mesh->mNumVertices);

    for (size_t i = 0; i < mesh->mNumVertices; ++i) {
        auto pos = mesh->mVertices[i];
        QVector3D vertex(pos.x, pos.y, pos.z);
        vertices.push_back(vertex);

        auto normal = mesh->mNormals[i];
        normals.push_back(QVector3D(normal.x, normal.y, normal.z).normalized());
    }

    indices.reserve(mesh->mNumFaces * 3);

    for (size_t i = 0; i < mesh->mNumFaces; ++i) {
        auto face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if(mesh->HasBones()) { //bones included in file
        skeleton= Skeleton(mesh->mNumBones, mesh->mNumVertices, mesh->mBones);
    }

    else //bones in another file, let's find weights and skeleton files, format skel1.*
    {
        std::string skelFile = fileName.substr(0, fileName.find_last_of("/")+1);
        //std::string weightFile = skelFile + "skel1.weights";
        //skelFile += "skel1.skeleton";

        skeleton = Skeleton(skelFile);//, weightFile);
    }

    return Mesh(vertices, indices, skeleton);
}

Mesh Mesh::fromCustomFile(const std::string &meshFileName,
                          const std::string &skelFileName,
                          const std::string &weightFileName)
{
    std::ifstream meshFile { meshFileName };

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
                    indices.push_back(face[i])
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

    Skeleton skeleton { };
    skeleton.parseSkelFile(skelFileName);
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

void Mesh::computeCoRs() {
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
        CoRs.push_back(c/s);
    }
}
