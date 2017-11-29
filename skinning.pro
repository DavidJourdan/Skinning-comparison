#-------------------------------------------------
#
# Project created by QtCreator 2017-10-21T20:14:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

macx: {
    QMAKE_CXXFLAGS += -stdlib=libc++
}

TARGET = skinning
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    openglwidget.cpp \
    mesh.cpp \
    skeleton.cpp

HEADERS  += mainwindow.h \
    openglwidget.h \
    mesh.h \
    skeleton.h \
    vertex.h

unix:!macx {
    LIBS += -lassimp
}

macx: {
    LIBS += -L/usr/local/lib -lassimp
    INCLUDEPATH += /usr/local/include
}

DISTFILES += \
    shader.vert \
    shader.frag \
    boneshader.frag \
    boneshader.vert

RESOURCES += \
    shaders.qrc
