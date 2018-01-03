#ifndef LBS_H
#define LBS_H

#include "base.h"

namespace view {

class Lbs : public Base
{
    Q_OBJECT
public:
    Lbs(Core *core, QWidget *parent);

protected:
    void initializeGL() final;

    void paintGL() final;

private:
    QOpenGLVertexArrayObject vao;

    QOpenGLShaderProgram prog;

    void setUpShader() final;

    void draw() final;
};

}

#endif // LBS_H
