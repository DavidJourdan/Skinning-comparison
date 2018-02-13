#ifndef SKELETON_H
#define SKELETON_H

#include <vector>
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
#include <iostream>
#include <fstream>

#include "dualquaternion.hpp"

using std::vector;
using std::pair;
using std::array;

struct Triangle {
    uint a;
    uint b;
    uint c;
};

struct Bone {
    QVector3D head;
    QVector3D tail;
    vector<uint32_t> childList;

    Bone(QVector3D h, QVector3D t, vector<uint32_t> cl):
        head(h), tail(t), childList(cl) {
    }
};

struct Weight {
    uint32_t boneIndex;
    float value;

    Weight(uint32_t bIdx, float val): boneIndex { bIdx }, value { val } { }
    Weight() { }
};

class Skeleton
{
public:
    Skeleton(vector<Bone> bl,
             vector<vector<Weight>> wll);

    const vector<Bone> &getBones() const { return bones; }
    const vector<vector<Weight>> getWeights() const { return weightListList; }
    const vector<QMatrix4x4> &getTransformations() const { return transformations; }
    const vector<QVector4D> &getQuaternions() const { return quaternions; }
    const vector<QVector4D> &getDQuatTransformationsNonDualPart() const { return transformationsDQNonDualPart; }
    const vector<QVector4D> &getDQuatTransformationsDualPart() const { return transformationsDQDualPart; }

    std::vector<QVector3D> getSkelLines() const;
    void rotateBone(const size_t boneIndex, float angle, const QVector3D &axis);

private:
    vector<Bone> bones;
    vector<vector<Weight>> weightListList;
    vector<QMatrix4x4> transformations;
    vector<QVector4D> quaternions;
    vector<QVector4D> transformationsDQNonDualPart;
    vector<QVector4D> transformationsDQDualPart;
};

#endif // SKELETON_H
