#include "mainwindow.h"

#include <QFileDialog>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto fileName = QFileDialog::getOpenFileName(nullptr, tr("Pick mesh"), QDir::homePath(), tr("Meshes (*.obj)"));

    glWidget = new OpenGLWidget(fileName.toStdString(), this);

    setCentralWidget(glWidget);
}

MainWindow::~MainWindow()
{

}
