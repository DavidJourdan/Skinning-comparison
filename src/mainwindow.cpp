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

    setupView();

    setupSkeleton();

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

void MainWindow::setupView()
{
    auto menu = menuBar()->addMenu(tr("&Affichage"));

    auto resetCamAction = new QAction { tr("Vue initiale") };
    resetCamAction->setShortcut(QKeySequence(tr("r")));

    connect(resetCamAction, &QAction::triggered, glWidget, &OpenGLWidget::resetCamera);

    menu->addAction(resetCamAction);

    auto toggleBoneAction = new QAction { tr("Afficher l'os sélectionné") };
    toggleBoneAction->setShortcut(QKeySequence(tr(" ")));

    connect(toggleBoneAction, &QAction::triggered, glWidget, &OpenGLWidget::toggleBoneActiv);

    menu->addAction(toggleBoneAction);

    auto focusBone = new QAction { tr("&Zoomer sur l'os sélectionné") };
    focusBone->setShortcut(QKeySequence(tr("z")));

    connect(focusBone, &QAction::triggered, glWidget, &OpenGLWidget::focusSelectedBone);
}

void MainWindow::setupSkeleton()
{
    auto menu = menuBar()->addMenu(tr("&Squelette"));

    auto selPrevBone = new QAction { tr("Sélectionner l'os précédent") };
    selPrevBone->setShortcut(Qt::Key_Left);

    connect(selPrevBone, &QAction::triggered, glWidget, &OpenGLWidget::selectPreviousBone);

    menu->addAction(selPrevBone);

    auto selNextBone = new QAction { tr("Sélectionner l'os suivant") };
    selNextBone->setShortcut(Qt::Key_Right);

    connect(selNextBone, &QAction::triggered, glWidget, &OpenGLWidget::selectNextBone);

    menu->addAction(selNextBone);
}
