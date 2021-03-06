#ifndef SKELETON_H
#define SKELETON_H

#include <vector>
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
#include <iostream>
#include <fstream>

#include "dualquaternion.hpp"

struct Triangle {
    uint a;
    uint b;
    uint c;
};

struct Bone {
    bool isEdge; // false if it's a PARENT_RELATION
    uint parent; // index of parent joint position
    uint child; // index of child joint position
    size_t successorNb;
    uint *successors; //array of Bone indices
    Bone(bool edge, uint parInd, uint childInd, size_t childNb = 0): isEdge(edge), 
        parent(parInd), child(childInd), successorNb(childNb) {
            successors = (successorNb != 0) ? new uint[successorNb] : nullptr;
    }
};

class Skeleton
{
public:
    Skeleton(const std::string &weightFile, const std::string &skelFile, size_t meshVertexCount);
    Skeleton();
    ~Skeleton();
    float simil(uint vertexInd, Triangle t);
    bool parseSkelFile(const std::string& file);
    std::vector<QVector3D> getSkelLines();
    size_t getEdgeNumber() {return edgeNb;}

    const std::vector<QVector3D> &getArticulations() const { return articulations; }
    const std::vector<Bone> &getBones() const { return bones; }

    void parseWeights(const std::string &fileName, size_t meshVertexCount);

    void rotateBone(const size_t boneIndex, float angle, const QVector3D &axis);

    float **getWeights() const { return weights; }
    unsigned **getBoneIndices() const { return boneInd; }
    size_t *getWeightsSize() const { return weightsSize; }

    const std::vector<QMatrix4x4> &getTransformations() const { return transformations; }
    const std::vector<QVector4D> &getQuaternions() const { return quaternions; }

    const std::vector<QVector4D> &getDQuatTransformationsNonDualPart() const {return transformationsDQNonDualPart;}
    const std::vector<QVector4D> &getDQuatTransformationsDualPart() const {return transformationsDQDualPart;}

private:
    float **weights;
    uint **boneInd;
    size_t *weightsSize;
    size_t edgeNb; // bones whose index is inferior to this number are EDGES, the  other are PARENT_RELATIONS

    std::vector<QVector3D> articulations;
    std::vector<Bone> bones;
    std::vector<QMatrix4x4> transformations;
    std::vector<QVector4D> quaternions;
    std::vector<QVector4D> transformationsDQNonDualPart;
    std::vector<QVector4D> transformationsDQDualPart;
    //std::vector<DualQuaternion> transformationsDQ;
};

#endif // SKELETON_H
