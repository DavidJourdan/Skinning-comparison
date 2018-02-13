#include "mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QAction>
#include <QActionGroup>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "view/lbs.h"
#include "view/dqs.h"
#include "view/cor.h"

#include "helpwindow.h"

MainWindow::MainWindow(const Config &config, QWidget *parent) : QMainWindow { parent },
    core { config }
{
    auto cen = new QWidget { this };

    setUpViews();

    setUpViewWidgets();

    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(lbsWidget);

    layout->addWidget(dqsWidget);

    layout->addWidget(corWidget);

    cen->setLayout(layout);

    setCentralWidget(cen);

    setUpDeform();

    setupView();

    setupSkeleton();

    setupMiscellaneous();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setUpViews()
{
    auto lbsView = new view::Lbs { &core, this };
    core.lbsView = lbsView;

    auto dqsView = new view::Dqs { &core, this };
    core.dqsView = dqsView;

    auto corView = new view::Cor { &core, this };
    core.corView = corView;
}

void MainWindow::setUpViewWidgets()
{
    const auto setUpWidget = [=](QWidget *&widget, view::Base *view, QString name) {
        widget = new QWidget { this };

        QSizePolicy policy { QSizePolicy::Preferred, QSizePolicy::Expanding };
        view->setSizePolicy(policy);

        auto layout = new QVBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        auto title = new QLabel { name };
        title->setAlignment(Qt::AlignHCenter);
        title->setMargin(5);

        layout->addWidget(title);

        layout->addWidget(view);

        widget->setLayout(layout);
    };

    setUpWidget(lbsWidget, core.lbsView, "Linear blend skinning");
    setUpWidget(dqsWidget, core.dqsView, "Dual quaternion skinning");
    setUpWidget(corWidget, core.corView, "Optimized centers of rotation");
}

void MainWindow::setUpDeform()
{
    const auto forceUpdate = [&] {
        core.update();
        QCoreApplication::processEvents();
        core.update();
    };

    auto lbsAction = new QAction { tr("&LBS"), this };
    lbsAction->setCheckable(true);

    connect(lbsAction, &QAction::toggled, [=](bool p) {
        lbsWidget->setVisible(p);
        forceUpdate();
    });

    lbsAction->setChecked(true);

    auto dqsAction = new QAction { tr("&DQS"), this };
    dqsAction->setCheckable(true);

    connect(dqsAction, &QAction::toggled, [=](bool p) {
        dqsWidget->setVisible(p);
        forceUpdate();
    });

    dqsWidget->setVisible(false);

    auto optimizedCorsAction = new QAction { tr("&Méthode de l'article"), this };
    optimizedCorsAction->setCheckable(true);

    connect(optimizedCorsAction, &QAction::toggled, [=](bool p) {
        corWidget->setVisible(p);
        forceUpdate();
    });

    corWidget->setVisible(false);

    auto group = new QActionGroup { this };

    group->addAction(lbsAction);
    group->addAction(dqsAction);
    group->addAction(optimizedCorsAction);

    auto deformMenu = menuBar()->addMenu(tr("&Méthode de déformation"));

    deformMenu->addAction(lbsAction);
    deformMenu->addAction(dqsAction);
    deformMenu->addAction(optimizedCorsAction);

    deformMenu->addSeparator();

    auto exclusive = new QAction { tr("Vues simultanées"), this };
    exclusive->setCheckable(true);

    connect(exclusive, &QAction::toggled, [=](bool p) {
        group->setExclusive(!p);
        if (!p) {
            const auto checked = group->checkedAction();

            lbsAction->setChecked(false);
            dqsAction->setChecked(false);
            optimizedCorsAction->setChecked(false);

            checked->setChecked(true);
        }
    });

    deformMenu->addAction(exclusive);
}

void MainWindow::setupMiscellaneous()
{
    auto menu = menuBar()->addMenu(tr("&Divers"));

    auto help = new QAction { tr("&Aide"), this };
    help->setShortcut(QKeySequence(tr("h")));

    connect(help, &QAction::triggered, [=] {
        openHelpWindow();
    });

    menu->addAction(help);

    auto quit = new QAction { tr("&Quitter l'application"), this };
    quit->setShortcuts({ QKeySequence::Quit, Qt::Key_Escape });

    connect(quit, &QAction::triggered, this, &MainWindow::close);

    menu->addAction(quit);
}

void MainWindow::setupView()
{
    auto menu = menuBar()->addMenu(tr("&Affichage"));

    auto resetCamAction = new QAction { tr("Vue initiale"), this };
    resetCamAction->setShortcut(QKeySequence(tr("r")));

    connect(resetCamAction, &QAction::triggered, [=] {
        core.resetCamera();
    });

    menu->addAction(resetCamAction);

    auto focusBone = new QAction { tr("&Zoomer sur l'os sélectionné"), this };
    focusBone->setShortcut(QKeySequence(tr("z")));

    connect(focusBone, &QAction::triggered, [=] {
        core.focusSelectedBone();
    });

    menu->addAction(focusBone);

    auto toggleMeshMode = new QAction { tr("&Mode creux"), this };
    toggleMeshMode->setShortcut(tr("m"));
    toggleMeshMode->setCheckable(true);

    connect(toggleMeshMode, &QAction::toggled, [=](bool p) {
        core.meshMode = p ? GL_LINE : GL_FILL;
        core.update();
    });

    menu->addAction(toggleMeshMode);

    auto toggleBoneDisplay = new QAction { tr("Afficher les &os"), this };
    toggleBoneDisplay->setShortcut(tr("o"));
    toggleBoneDisplay->setCheckable(true);
    toggleBoneDisplay->setChecked(true);

    connect(toggleBoneDisplay, &QAction::toggled, [=](bool p) {
        core.showBones = p;
        core.update();
    });

    menu->addAction(toggleBoneDisplay);

    auto toggleCorDisplay = new QAction { tr("Afficher/cacher les centres de ro&tation"), this };
    toggleCorDisplay->setShortcut(tr("t"));
    toggleCorDisplay->setCheckable(true);
    toggleCorDisplay->setChecked(true);

    connect(toggleCorDisplay, &QAction::toggled, [=](bool p) {
        core.showCors = p;
        core.update();
    });

    menu->addAction(toggleCorDisplay);
}

void MainWindow::setupSkeleton()
{
    auto menu = menuBar()->addMenu(tr("&Squelette"));

    auto pickBone = new QAction { tr("Sélectionner un os à la souris"), this };
    pickBone->setShortcut(tr("s"));

    connect(pickBone, &QAction::triggered, [=] {
        core.isPickingBone = true;

        const auto cursor = QCursor { Qt::CursorShape::CrossCursor };
        QGuiApplication::setOverrideCursor(cursor);
    });

    menu->addAction(pickBone);
}

void MainWindow::openHelpWindow() {
    HelpWindow *help = new HelpWindow();
    help->show();
}
