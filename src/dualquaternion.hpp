#ifndef DUALQUATERNION_HPP
#define DUALQUATERNION_HPP

#include <QVector4D>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QGenericMatrix>
#include <QQuaternion>
#include <iostream>

class DualQuaternion
{
public:
    DualQuaternion();
    DualQuaternion(QQuaternion nonDual, QQuaternion dual);
    DualQuaternion(const DualQuaternion& dq) {nonDualPart = dq.getNonDualPart(); dualPart = dq.getDualPart();}

    static DualQuaternion transformMatrixToDQuat(QMatrix4x4 mat);

    QQuaternion getNonDualPart() const;
    QQuaternion getDualPart() const;

    DualQuaternion& operator *=(const QQuaternion& other);
    DualQuaternion& operator *=(const DualQuaternion& other);

    void normalize();

private:
    QQuaternion nonDualPart;
    QQuaternion dualPart;
};

DualQuaternion operator *(DualQuaternion left, const QQuaternion& right);
DualQuaternion operator *(DualQuaternion left, const DualQuaternion& right);

#endif // DUALQUATERNION_HPP
