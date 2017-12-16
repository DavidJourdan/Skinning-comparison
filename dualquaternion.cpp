#include "dualquaternion.hpp"

DualQuaternion::DualQuaternion()
{

}

DualQuaternion::DualQuaternion(QQuaternion nonDual, QQuaternion dual) : nonDualPart(nonDual), dualPart(dual)
{
}

DualQuaternion DualQuaternion::transformMatrixToDQuat(QMatrix4x4 mat)
{
    QMatrix3x3 rotMatrix;
    rotMatrix(0, 0) = mat(0, 0);
    rotMatrix(0, 1) = mat(0, 1);
    rotMatrix(0, 2) = mat(0, 2);
    rotMatrix(1, 0) = mat(1, 0);
    rotMatrix(1, 0) = mat(1, 1);
    rotMatrix(1, 0) = mat(1, 2);
    rotMatrix(2, 0) = mat(2, 0);
    rotMatrix(2, 0) = mat(2, 1);
    rotMatrix(2, 0) = mat(2, 2);

    QQuaternion qRotation = QQuaternion::fromRotationMatrix(rotMatrix);

    QVector3D translateVector = mat.column(3).toVector3D();

    DualQuaternion translationDQ(QQuaternion(QVector4D(1, 0, 0, 0)),
                                 0.5*QQuaternion(QVector4D(0, translateVector.x(), translateVector.y(), translateVector.z()) ));


    return transformDQFromRotAndTransl(translationDQ, qRotation);
}

DualQuaternion DualQuaternion::transformDQFromRotAndTransl(DualQuaternion t, QQuaternion q)
{
    QQuaternion tDual = t.getDualPart();
    return DualQuaternion(q, tDual*q);
}
