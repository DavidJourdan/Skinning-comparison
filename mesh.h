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

    std::vector<QVector3D>& getVertices() { return vertices; }
    std::vector<unsigned>& getIndices() { return indices;}
    std::vector<QVector3D>& getNormals() {return normals;}
    std::vector<QVector3D> getSkelLines() { return skeleton.getSkelLines(); }
    uint getNumberBones() {return skeleton.getNumberBones();}
    void computeCoRs(void);

private:
    std::vector<QVector3D> vertices;
    std::vector<QVector3D> normals;
    std::vector<unsigned> indices;
    std::vector<QVector3D> CoRs;
    Skeleton skeleton;
    float area(Triangle t);
};

#endif // MESH_H
