#include "mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QAction>
#include <QMenuBar>

MainWindow::MainWindow(const Config &config, QWidget *parent) : QMainWindow { parent }
{
    glWidget = new OpenGLWidget(config, this);

    setCentralWidget(glWidget);

    QAction *lbsAction = new QAction { tr("&LBS"), this };
    connect(lbsAction, &QAction::triggered, glWidget, &OpenGLWidget::deformWithLbs);

    QAction *optimizedCorsAction = new QAction { tr("&Méthode de l'article"), this };
    connect(optimizedCorsAction, &QAction::triggered, glWidget, &OpenGLWidget::deformWithOptimizedCors);

    auto deformMenu = menuBar()->addMenu(tr("&Méthode de déformation"));
    deformMenu->addAction(lbsAction);
    deformMenu->addAction(optimizedCorsAction);
}

MainWindow::~MainWindow()
{

}
