#include "dualquaternion.hpp"

DualQuaternion::DualQuaternion() : dualPart(0, 0, 0, 0)
{

}

DualQuaternion::DualQuaternion(QQuaternion nonDual, QQuaternion dual) : nonDualPart(nonDual), dualPart(dual)
{
}

DualQuaternion DualQuaternion::transformMatrixToDQuat(QMatrix4x4 mat)
{
    QMatrix3x3 rotMatrix;
    for(int i = 0 ; i < 3 ; i++)
    {
        for(int j = 0 ; j < 3 ; j++)
        {
            rotMatrix(i,j) = mat(i,j);
        }
    }

    QQuaternion qRotation = QQuaternion::fromRotationMatrix(rotMatrix);
    qRotation.normalize();

    QVector3D translateVector = mat.column(3).toVector3D();

    //std::cout << translateVector.x() << " " << translateVector.y() << " " << translateVector.z() << std::endl;

    DualQuaternion translationDQ(QQuaternion(1.0f, 0.0f, 0.0f, 0.0f),
                                 0.5f*QQuaternion(0.0f, translateVector.x(), translateVector.y(), translateVector.z()));


    QQuaternion tDual = translationDQ.getDualPart();
    tDual = qRotation * qRotation;
    return DualQuaternion(qRotation, tDual);
}

DualQuaternion& DualQuaternion::operator *=(const QQuaternion& other)
{
    QQuaternion newNonDual = nonDualPart * other, newDual = dualPart * other;
    nonDualPart = newNonDual;
    dualPart = newDual;
    return *this;
}

DualQuaternion& DualQuaternion::operator *=(const DualQuaternion& other)
{
    QQuaternion newNonDual = nonDualPart * other.getNonDualPart();
    QQuaternion newDual = nonDualPart * other.getDualPart() + dualPart * other.getNonDualPart();
    nonDualPart = newNonDual;
    dualPart = newDual;
    return *this;
}

DualQuaternion operator *(DualQuaternion left, const QQuaternion& right)
{
    return left *= right;
}

DualQuaternion operator *(DualQuaternion left, const DualQuaternion& right)
{
    return left *= right;
}

QQuaternion DualQuaternion::getNonDualPart() const
{
    return nonDualPart;
}

QQuaternion DualQuaternion::getDualPart() const
{
    return dualPart;
}

void DualQuaternion::normalize()
{
    float norm = nonDualPart.length();

    nonDualPart /= norm;
    dualPart /= norm;

    dualPart = dualPart - nonDualPart * QQuaternion::dotProduct(nonDualPart, dualPart);
}
