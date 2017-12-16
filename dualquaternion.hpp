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

    static DualQuaternion transformMatrixToDQuat(QMatrix4x4 mat);

    static DualQuaternion transformDQFromRotAndTransl(DualQuaternion t, QQuaternion q);

    inline QQuaternion getNonDualPart() {return nonDualPart;}
    inline QQuaternion getDualPart() {return dualPart;}

private:
    QQuaternion nonDualPart;
    QQuaternion dualPart;
};

#endif // DUALQUATERNION_HPP
