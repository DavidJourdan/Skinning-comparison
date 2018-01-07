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
    QWidget *dqsWidget;
    QWidget *corWidget;

    void setUpViews();
    void setUpViewWidgets();

    void setUpDeform();
    void setupMiscellaneous();
    void setupView();
    void setupSkeleton();

    void openHelpWindow();
};

#endif // MAINWINDOW_H
