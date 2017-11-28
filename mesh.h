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
    std::vector<unsigned>& getIndices() { return indices;}
    std::vector<QVector3D>& getNormals() {return normals;}
    void computeCoRs(void);
private:
    std::vector<Vertex> vertices;
    std::vector<QVector3D> normals;
    std::vector<unsigned> indices;
    std::vector<Vertex> CoRs;
    Skeleton skeleton;
    float area(Triangle t);
};

#endif // MESH_H
