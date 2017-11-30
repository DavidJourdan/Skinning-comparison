#include "mainwindow.h"
#include "config.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QCommandLineParser parser;

    QCommandLineOption inputFileOption("i", "File to read from", "inputFile");
    parser.addOption(inputFileOption);

    QCommandLineOption skelFileOption { "s", "File to read skeleton from",
                                        "skelFile" };
    parser.addOption(skelFileOption);

    QCommandLineOption weightFileOption { "w", "File to read weigths from",
                                        "weightFile" };
    parser.addOption(weightFileOption);

    parser.process(a);

    auto inputFile = parser.value(inputFileOption).toStdString();
    auto skelFile = parser.value(skelFileOption).toStdString();
    auto weightFile = parser.value(weightFileOption).toStdString();

    Config config { inputFile, skelFile, weightFile };

    MainWindow w(config);
    w.showMaximized();

    return a.exec();
}
