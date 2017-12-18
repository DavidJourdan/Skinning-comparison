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
    size_t getEdgeNumber() {return skeleton.getEdgeNumber();}
    std::vector<QVector3D> computeCoRs(void);
    QVector3D computeCoR(uint i);
    uint getBoneSelected() const {return boneSelected;}
    void setBoneSelected(uint i) {boneSelected = i%skeleton.getEdgeNumber();}

    const std::vector<QVector3D> &getArticulations() const
    {
        return skeleton.getArticulations();
    }

    const std::vector<Bone> &getBones() const { return skeleton.getBones(); }

    static Mesh fromGenericFile(const std::string &fileName);
    static Mesh fromCustomFile(const Config &config);

    void rotateBone(float angle, QVector3D axis);

    float **getWeights() { return skeleton.getWeights(); }
    unsigned **getBoneIndices() { return skeleton.getBoneIndices(); }
    const std::vector<QMatrix4x4> &getTransformations() const { return skeleton.getTransformations(); }
    const std::vector<QVector4D> &getQuaternions() const { return skeleton.getQuaternions(); }

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
