#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    glWidget = new OpenGLWidget("/home/rdesplanques/Projects/skinning/data/mesh.obj", this);

    setCentralWidget(glWidget);
}

MainWindow::~MainWindow()
{

}
