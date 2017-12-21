#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "openglwidget.h"
#include "config.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const Config &config, QWidget *parent = 0);
    ~MainWindow();

private:
    OpenGLWidget *glWidget;

    void setupMiscellaneous();
    void setupView();
};

#endif // MAINWINDOW_H
