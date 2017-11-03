#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "openglwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString fileName = "", QWidget *parent = 0);
    ~MainWindow();

private:
    OpenGLWidget *glWidget;
};

#endif // MAINWINDOW_H
