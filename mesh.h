#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include<QVector3D>
#include "skeleton.h"
#include "config.h"

class Mesh
{
public:
    Mesh(std::vector<QVector3D> vertices,
         std::vector<unsigned> indices,
         Skeleton skeleton);

    Mesh(std::vector<QVector3D> vertices,
         std::vector<unsigned> indices,
         std::vector<QVector3D> normals,
         Skeleton skeleton);

    std::vector<QVector3D>& getVertices() { return vertices; }
    std::vector<unsigned>& getIndices() { return indices;}
    std::vector<QVector3D>& getNormals() {return normals;}
    std::vector<QVector3D> getSkelLines() { return skeleton.getSkelLines(); }
    uint getNumberBones() {return skeleton.getNumberBones();}
    std::vector<QVector3D> computeCoRs(void);
    QVector3D computeCoR(uint i);
    uint getBoneSelected() {return boneSelected;}
    void setBoneSelected(uint i) {boneSelected = i%skeleton.getNumberBones();}

    const std::vector<QVector3D> &getArticulations() const
    {
        return skeleton.getArticulations();
    }

    const std::vector<Bone> &getEdges() const { return skeleton.getEdges(); }

    static Mesh fromGenericFile(const std::string &fileName);
    static Mesh fromCustomFile(const Config &config);

    void rotateBone(float angle, QVector3D axis);

    float **getWeights() { return skeleton.getWeights(); }
    unsigned **getBoneIndices() { return skeleton.getBoneIndices(); }

private:
    std::vector<QVector3D> vertices;
    std::vector<unsigned> indices;
    std::vector<QVector3D> normals;
    std::vector<QVector3D> CoRs;
    Skeleton skeleton;
    uint boneSelected { 0 };
    float area(Triangle t);
};

#endif // MESH_H
