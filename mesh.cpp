#include "mesh.h"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Mesh::Mesh(const std::string &fileName)
{
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(fileName, 0);

    if (!scene) {
        std::cerr << importer.GetErrorString();

        // Maybe throw an exception here.
        std::exit(EXIT_FAILURE);
    }

    // For testing purpose, just trying to get one mesh here.
    const auto mesh = scene->mMeshes[0];

    vertices.reserve(mesh->mNumVertices);
    CoRs.reserve(mesh->mNumVertices);

    for (size_t i = 0; i < mesh->mNumVertices; ++i) {
        auto pos = mesh->mVertices[i];
        Vertex vertex(pos.x, pos.y, pos.z);
        vertices.push_back(vertex);
    }

    for (size_t i = 0; i < mesh->mNumFaces; ++i) {
        auto face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if(mesh->mBones) {
        skeleton= Skeleton(mesh->mNumBones, mesh->mNumVertices, mesh->mBones);
    }
}

float Mesh::area(Triangle t) {
    Vertex v = (vertices[t.b]-vertices[t.a]).cross(vertices[t.c]-vertices[t.a]);
    return v.len()/2.;
}

void Mesh::computeCoRs() {
    // subdivide mesh
    // compute CoRs
    for(uint i= 0; i < vertices.size(); i++) {
        Vertex c;
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