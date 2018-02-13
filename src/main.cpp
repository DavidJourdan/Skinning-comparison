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
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QCommandLineParser parser;

    QCommandLineOption inputFileOption(QStringList() << "i" << "input", "File to read from", "inputFile");
    parser.addOption(inputFileOption);

    parser.process(a);

    const auto inputFile = parser.value(inputFileOption).toStdString();

    using std::move;
    Config config { move(inputFile) };

    MainWindow w(config);
    w.showMaximized();

    return a.exec();
}
