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

    for (size_t i = 0; i < mesh->mNumVertices; ++i) {
        glm::vec3 pos;
        auto oPos = mesh->mVertices[i];
        pos.x = oPos.x;
        pos.y = oPos.y;
        pos.z = oPos.z;
        vertices.push_back(pos);
    }

    for (size_t i = 0; i < mesh->mNumFaces; ++i) {
        auto face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }
}
