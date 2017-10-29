#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include<QVector3D>
#include "skeleton.h"
#include "vertex.h"

class Mesh
{
public:
    Mesh(const std::string& fileName);

    std::vector<Vertex>& getVertices() { return vertices; }
    std::vector<unsigned>& getIndices() {
        return indices;
    }

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
    Skeleton skeleton;
};

#endif // MESH_H
