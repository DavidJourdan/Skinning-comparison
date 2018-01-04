#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include "config.h"
#include "core.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const Config &config, QWidget *parent = 0);
    ~MainWindow();

private:
    Core core;

    QWidget *lbsWidget;

    void setUpViewWidgets();

    void setUpDeform();
    void setupMiscellaneous();
    void setupView();
    void setupSkeleton();
};

#endif // MAINWINDOW_H
