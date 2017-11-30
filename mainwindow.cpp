#include "mainwindow.h"

#include <QFileDialog>
#include <QDir>

MainWindow::MainWindow(QString fileName, QString skelFile, QString weightFile, QWidget *parent)
    : QMainWindow(parent)
{
    if (fileName == "") {
        fileName = QFileDialog::getOpenFileName(nullptr, tr("Pick mesh"), QDir::homePath(), tr("Meshes (*.obj)"));
    }

    glWidget = new OpenGLWidget(fileName.toStdString(), this);

    setCentralWidget(glWidget);
}

MainWindow::~MainWindow()
{

}
