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
    Mesh(std::vector<QVector3D> vertices,
         std::vector<QVector3D> indices,
         Skeleton skeleton);

    std::vector<QVector3D>& getVertices() { return vertices; }
    std::vector<unsigned>& getIndices() { return indices;}
    std::vector<QVector3D>& getNormals() {return normals;}
    std::vector<QVector3D> getSkelLines() { return skeleton.getSkelLines(); }
    uint getNumberBones() {return skeleton.getNumberBones();}
    void computeCoRs(void);
    uint getBoneSelected() {return boneSelected;}
    void setBoneSelected(uint i) {boneSelected = i%skeleton.getNumberBones();}
    static Mesh fromGenericFile(const std::string &fileName);
    static Mesh fromCustomFile(const std::string &meshFileName,
                               const std::string &skelFileName,
                               const std::string &weightFileName);

private:
    std::vector<QVector3D> vertices;
    std::vector<QVector3D> normals;
    std::vector<unsigned> indices;
    std::vector<QVector3D> CoRs;
    Skeleton skeleton;
    uint boneSelected;
    float area(Triangle t);
};

#endif // MESH_H
