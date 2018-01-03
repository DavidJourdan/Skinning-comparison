#ifndef DQS_H
#define DQS_H

#include "base.h"

namespace view {

class Dqs : public Base
{
    Q_OBJECT
public:
    Dqs(Core *core, QWidget *parent);

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

#endif // DQS_H
