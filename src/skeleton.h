#ifndef SKELETON_H
#define SKELETON_H

#include <assimp/scene.h>
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
    QVector3D head; // index of parent joint position
    QVector3D tail; // index of child joint position
    vector<uint32_t> childList;

    Bone(QVector3D h, QVector3D t, vector<uint32_t> cl):
        head(h), tail(t), childList(cl) {
    }
};

struct Weight {
    uint32_t boneIndex;
    float value;

    Weight(uint32_t bIdx, float val): boneIndex { bIdx }, value { val } { }
};

using std::vector;
using std::pair;
using std::array;

class Skeleton
{
public:
    Skeleton(vector<Bone> bl,
             vector<vector<Weight>> wll);
    std::vector<QVector3D> getSkelLines();

    const std::vector<Bone> &getBones() const { return bones; }

    void rotateBone(const size_t boneIndex, float angle, const QVector3D &axis);

    const std::vector<QMatrix4x4> &getTransformations() const { return transformations; }
    const std::vector<QVector4D> &getQuaternions() const { return quaternions; }

    const std::vector<QVector4D> &getDQuatTransformationsNonDualPart() const {return transformationsDQNonDualPart;}
    const std::vector<QVector4D> &getDQuatTransformationsDualPart() const {return transformationsDQDualPart;}

private:
    vector<vector<Weight>> weightListList;

    std::vector<QVector3D> articulations;
    std::vector<Bone> bones;
    std::vector<QMatrix4x4> transformations;
    std::vector<QVector4D> quaternions;
    std::vector<QVector4D> transformationsDQNonDualPart;
    std::vector<QVector4D> transformationsDQDualPart;
    //std::vector<DualQuaternion> transformationsDQ;
};

#endif // SKELETON_H
