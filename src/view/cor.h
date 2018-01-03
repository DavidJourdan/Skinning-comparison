#ifndef COR_H
#define COR_H

#include "base.h"

namespace view {

class Cor : public Base
{
    Q_OBJECT
public:
    Cor(Core *core, QWidget *parent);

protected:
    void initializeGL() final;

    void paintGL() final;

private:
    QOpenGLVertexArrayObject vao;
    QOpenGLVertexArrayObject pointVao;

    QOpenGLShaderProgram prog;
    QOpenGLShaderProgram pointProg;

    void setUpShader() final;
    void setUpPoints();

    void draw() final;
    void drawPoints();
};

}

#endif // COR_H
