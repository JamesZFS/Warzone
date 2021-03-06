#-------------------------------------------------
#
# Project created by QtCreator 2019-05-28T23:08:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Warzone
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    gamesystem.cpp \
    soldier.cpp \
    constants.cpp \
    engine.cpp \
    land.cpp \
    actor.cpp \
    weapon.cpp \
    contactlistener.cpp \
    frontsight.cpp \
    explosioncallback.cpp \
    explosioneffect.cpp \
    brick.cpp \
    initializer.cpp

HEADERS += \
        mainwindow.h \
    gamesystem.h \
    defs.h \
    soldier.h \
    constants.h \
    engine.h \
    land.h \
    actor.h \
    weapon.h \
    contactlistener.h \
    frontsight.h \
    explosioneffect.h \
    explosioncallback.h \
    brick.h \
    initializer.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += \
    /Users/james/Test/liquidfun-master/liquidfun/Box2D

LIBS += \
    /Users/james/Test/liquidfun-master/liquidfun/Box2D/Box2D/Debug/libliquidfun.a

RESOURCES += \
    res.qrc
