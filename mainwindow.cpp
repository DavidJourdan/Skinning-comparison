#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    glWidget = new OpenGLWidget("data/mesh.obj", this);

    setCentralWidget(glWidget);
}

MainWindow::~MainWindow()
{

}
