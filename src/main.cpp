#include "mainwindow.h"
#include "config.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <iostream>

void precomputeRun(const QCommandLineParser &parser) {
    if (!parser.isSet("inputFile")) {
        std::cerr << "No input file specified\n";
        std::exit(EXIT_FAILURE);
    }

    if (parser.isSet("s")) {
        auto inputFile = parser.value("i").toStdString();
        auto skelFile = parser.value("s").toStdString();
        auto weightFile = parser.value("w").toStdString();

        Config config { inputFile, skelFile, weightFile };
    } else {
        std::cerr << "No skeleton file specified\n";
        std::exit(EXIT_FAILURE);
    }

    auto mesh = Mesh::fromCustomFile(config);
    mesh.computeCoRs();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QCommandLineParser parser;

    QCommandLineOption precomputeOption { QStringList << "p" << "precompute",
                "GUI mode or compute mode",
                "precompute"};

    QCommandLineOption inputFileOption(QStringList << "i" << "input", "File to read from", "inputFile");
    parser.addOption(inputFileOption);

    QCommandLineOption outputFileOption(QStringList << "o" << "output", "File to write to", "outputFile");

    QCommandLineOption skelFileOption { "s", "File to read skeleton from",
                                        "skelFile" };
    parser.addOption(skelFileOption);

    QCommandLineOption weightFileOption { "w", "File to read weights from",
                                        "weightFile" };
    parser.addOption(weightFileOption);

    parser.process(a);

    const auto precomputeMode = parser.isSet(precomputeOption);

    auto inputFile = parser.value(inputFileOption).toStdString();
    auto skelFile = parser.value(skelFileOption).toStdString();
    auto weightFile = parser.value(weightFileOption).toStdString();

    Config config { inputFile, skelFile, weightFile };

    MainWindow w(config);
    w.showMaximized();

    return a.exec();
}
