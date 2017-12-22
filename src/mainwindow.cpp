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
    auto menu = menuBar()->addMenu(tr("&Divers"));

    auto computeCors = new QAction { tr("&Calculer les centres de rotation") };
    computeCors->setShortcut(QKeySequence(tr("c")));

    connect(computeCors, &QAction::triggered, glWidget, &OpenGLWidget::computeCoRs);

    menu->addAction(computeCors);

    auto quit = new QAction { tr("&Quitter l'application") };
    quit->setShortcuts({ QKeySequence::Quit, Qt::Key_Escape });

    connect(quit, &QAction::triggered, this, &MainWindow::close);

    menu->addAction(quit);
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

    menu->addAction(focusBone);

    auto toggleMeshMode = new QAction { tr("&Alterner mode plein/creux") };
    toggleMeshMode->setShortcut(tr("m"));

    connect(toggleMeshMode, &QAction::triggered, glWidget, &OpenGLWidget::toggleMeshMode);

    menu->addAction(toggleMeshMode);
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
