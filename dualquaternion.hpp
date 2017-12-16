#ifndef DUALQUATERNION_HPP
#define DUALQUATERNION_HPP

#include <QVector4D>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QGenericMatrix>
#include <QQuaternion>

class DualQuaternion
{
public:
    DualQuaternion();
    DualQuaternion(QQuaternion nonDual, QQuaternion dual);
    DualQuaternion(const DualQuaternion& dq) {nonDualPart = dq.getNonDualPart(); dualPart = dq.getDualPart();}

    static DualQuaternion transformMatrixToDQuat(QMatrix4x4 mat);

    static DualQuaternion transformDQFromRotAndTransl(DualQuaternion t, QQuaternion q);

    QQuaternion getNonDualPart() const;
    QQuaternion getDualPart() const;

private:
    QQuaternion nonDualPart;
    QQuaternion dualPart;
};

#endif // DUALQUATERNION_HPP
