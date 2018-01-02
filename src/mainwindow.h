#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include "openglwidget.h"
#include "config.h"
#include "core.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const Config &config, QWidget *parent = 0);
    ~MainWindow();

private:
    OpenGLWidget *glWidget;
    Core core;

    void setupMiscellaneous();
    void setupView();
    void setupSkeleton();
};

#endif // MAINWINDOW_H
