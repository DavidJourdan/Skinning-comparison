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
MOC_DIR = ./tmp/moc
OBJECTS_DIR = ./tmp/obj

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/openglwidget.cpp \
    src/mesh.cpp \
    src/skeleton.cpp

HEADERS  += src/mainwindow.h \
    src/openglwidget.h \
    src/mesh.h \
    src/skeleton.h \
    src/config.h

unix:!macx {
    LIBS += -lassimp
}

macx: {
    LIBS += -L/usr/local/lib -lassimp
    INCLUDEPATH += /usr/local/include
}

DISTFILES += \
    shaders/shader.vert \
    shaders/shader.frag \
    shaders/boneshader.frag \
    shaders/boneshader.vert

RESOURCES += \
    shaders/shaders.qrc
