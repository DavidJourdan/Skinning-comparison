#include "skeleton.h"
#include <math.h>
#include <locale>
#include <sstream>
#define SIGMA_2 0.01

using std::array;
using std::pair;
using std::vector;

Skeleton::Skeleton(vector<Bone> bl, vector<vector<Weight> > wll) :
    bones { bl }, weightListList { wll }, transformations(bl.size()),
    quaternions(bl.size(), QVector4D(0.0f, 0.0f, 0.0f, 1.0f)),
    transformationsDQNonDualPart(bl.size(), QVector4D(0.0f, 0.0f, 0.0f, 1.0f)),
    transformationsDQDualPart(bl.size())
{

}

vector<QVector3D> Skeleton::getSkelLines() const {
    vector<QVector3D> lines;

    for(Bone b : bones) {
        lines.push_back(b.head);
        lines.push_back(b.tail);
    }

    return lines;
}

void Skeleton::rotateBone(const size_t boneIndex, float angle, const QVector3D &axis)
{
    QMatrix4x4 transform { };
    
    const auto h = bones[boneIndex].head;
 
    transform.translate(h);
    transform.rotate(angle, axis);
    transform.translate(-h);

    QQuaternion quaternion = QQuaternion::fromAxisAndAngle(axis, angle);

    DualQuaternion trans(QQuaternion(1, 0, 0, 0), 0.5*QQuaternion(0, h));
    DualQuaternion transInv(QQuaternion(1, 0, 0, 0), 0.5* QQuaternion(0, -h));
    QQuaternion rot = QQuaternion::fromAxisAndAngle(axis, angle).normalized();

    DualQuaternion transfoDQ = (trans * rot) * transInv;

    trans.normalize();
 
    // depth-first search in the skeleton tree
    std::vector<uint> stack;
    stack.push_back(boneIndex);
 
    while (!stack.empty()) {
        uint bIdx = stack.back();
        stack.pop_back();

        transformations[bIdx] = transform * transformations[bIdx];
        QQuaternion quat = quaternion * QQuaternion(quaternions[bIdx]);
        quaternions[bIdx] = quat.toVector4D();

        DualQuaternion dq = transfoDQ * DualQuaternion(QQuaternion(transformationsDQNonDualPart[bIdx]), QQuaternion(transformationsDQDualPart[bIdx]));
        transformationsDQNonDualPart[bIdx] = dq.getNonDualPart().toVector4D();
        transformationsDQDualPart[bIdx] = dq.getDualPart().toVector4D();

        auto &head = bones[bIdx].head;
        head = transform * head;
        auto &tail = bones[bIdx].tail;
        tail = transform * tail;

        for (const auto c : bones[bIdx].childList) {
            stack.push_back(c);
        }
    }
}
