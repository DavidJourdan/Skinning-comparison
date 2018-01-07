#pragma once 

#include <QMainWindow>
#include <QTextEdit>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QDir>

class HelpWindow : public QMainWindow
{
    Q_OBJECT

public:
    HelpWindow(QWidget *parent = 0) {
        this->resize(400,300);
        this->setWindowTitle(tr("Aide"));
        textWidget = new QTextEdit(this);
        textWidget->setReadOnly(true);
        setCentralWidget(textWidget);
        readFromFile();
    }
    ~HelpWindow() {
    }

private:
    QTextEdit *textWidget;
    void readFromFile() {
        QString filename = "help.txt";
        QFile file(QDir::currentPath() + "/ressources/" + filename);
        if(!file.exists()) {
            qDebug() << "Error : " << QDir::currentPath() + filename << " not found\n";
        }
        textWidget->clear();
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream stream(&file);
            while (!stream.atEnd()){
                textWidget->setText(textWidget->toPlainText() + stream.readLine() + "\n");
            }
        }
        file.close();
    }
};
