#include "mainwindow.h"
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

    parser.process(a);

    auto inputFile = parser.value(inputFileOption);

    MainWindow w(inputFile);
    w.showMaximized();

    return a.exec();
}
