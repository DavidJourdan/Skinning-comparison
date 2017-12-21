#include "mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>

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

    auto deformLabel = new QLabel { this };
    deformLabel->setText("Linear blend skinning");

    connect(lbsAction, &QAction::triggered, [=] {
        deformLabel->setText("Linear blend skinning");
    });

    connect(dqsAction, &QAction::triggered, [=] {
        deformLabel->setText("Dual quaternion skinning");
    });

    connect(optimizedCorsAction, &QAction::triggered, [=] {
        deformLabel->setText("Méthode de l'article");
    });

    statusBar()->addWidget(deformLabel);

    setupMiscellaneous();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setupMiscellaneous()
{
    auto miscMenu = menuBar()->addMenu(tr("&Divers"));

    auto corAction = new QAction { tr("&Calculer les centres de rotation") };
    corAction->setShortcut(QKeySequence(tr("c")));

    connect(corAction, &QAction::triggered, glWidget, &OpenGLWidget::computeCoRs);

    miscMenu->addAction(corAction);
}
