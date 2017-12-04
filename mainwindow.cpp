#include "mainwindow.h"

#include <QFileDialog>
#include <QDir>

MainWindow::MainWindow(const Config &config, QWidget *parent) : QMainWindow { parent }
{
    glWidget = new OpenGLWidget(config, this);

    setCentralWidget(glWidget);
}

MainWindow::~MainWindow()
{

}
