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
    src/view/base.cpp \
    src/mesh.cpp \
    src/skeleton.cpp \
    src/dualquaternion.cpp \
    src/core.cpp \
    src/view/lbs.cpp \
    src/view/dqs.cpp

HEADERS  += src/mainwindow.h \
    src/view/base.h \
    src/mesh.h \
    src/skeleton.h \
    src/config.h \
    src/dualquaternion.hpp \
    src/core.h \
    src/view/lbs.h \
    src/view/dqs.h

unix:!macx {
    LIBS += -lassimp
}

macx: {
    LIBS += -L/usr/local/lib -lassimp
    INCLUDEPATH += /usr/local/include
}

DISTFILES += \
    shaders/lbs_shader.vert \
    shaders/boneshader.frag \
    shaders/boneshader.vert \
    shaders/optimized_cors.frag \
    shaders/optimized_cors.vert \
    shaders/pointshader.frag \
    shaders/pointshader.vert \
    shaders/dqshader.vert \
    shaders/dqshader.vert \
    shaders/shader.frag

RESOURCES += \
    shaders/shaders.qrc
