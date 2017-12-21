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

    QAction *dqsAction = new QAction { tr("&DQS") };
    connect(dqsAction, &QAction::triggered, glWidget, &OpenGLWidget::deformWithDqs);

    QAction *optimizedCorsAction = new QAction { tr("&Méthode de l'article"), this };
    connect(optimizedCorsAction, &QAction::triggered, glWidget, &OpenGLWidget::deformWithOptimizedCors);

    auto deformMenu = menuBar()->addMenu(tr("&Méthode de déformation"));
    deformMenu->addAction(lbsAction);
    deformMenu->addAction(dqsAction);
    deformMenu->addAction(optimizedCorsAction);
}

MainWindow::~MainWindow()
{

}
