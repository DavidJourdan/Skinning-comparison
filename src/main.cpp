#include "mainwindow.h"
#include "config.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <iostream>

void precomputeRun(const Config &config) {
    auto mesh = Mesh::fromCustomFile(config);
    mesh.computeCoRs();
    mesh.writeToFile(config.outputFile);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QCommandLineParser parser;

    QCommandLineOption precomputeOption { QStringList() << "p" << "precompute",
                "GUI mode or compute mode"};
    parser.addOption(precomputeOption);

    QCommandLineOption inputFileOption(QStringList() << "i" << "input", "File to read from", "inputFile");
    parser.addOption(inputFileOption);

    QCommandLineOption outputFileOption(QStringList() << "o" << "output", "File to write to", "outputFile");
    parser.addOption(outputFileOption);

    QCommandLineOption skelFileOption { "s", "File to read skeleton from",
                                        "skelFile" };
    parser.addOption(skelFileOption);

    QCommandLineOption weightFileOption { "w", "File to read weights from",
                                        "weightFile" };
    parser.addOption(weightFileOption);

    parser.process(a);

    const auto inputFile = parser.value(inputFileOption).toStdString();
    const auto skelFile = parser.value(skelFileOption).toStdString();
    const auto weightFile = parser.value(weightFileOption).toStdString();
    const auto outputFile = parser.value(outputFileOption).toStdString();
    const auto mode = parser.isSet(precomputeOption) ? Config::Mode::precompute : Config::Mode::gui;

    Config config { inputFile, skelFile, weightFile, outputFile, mode };

    switch (config.mode) {
    case Config::Mode::precompute:
        precomputeRun(config);
        return EXIT_SUCCESS;
        break;
    case Config::Mode::gui:
        break;
    default:
        break;
    }

    MainWindow w(config);
    w.showMaximized();

    return a.exec();
}
