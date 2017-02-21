#-------------------------------------------------
#
# Project created by QtCreator 2017-01-11T17:00:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gantt-full-demo
TEMPLATE = app

include(../gantt-lib-include.pri)
include(../demo-lib-include.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    gantttreemodel.cpp \
    modelwrapper.cpp

HEADERS  += mainwindow.h \
    gantttreemodel.h \
    modelwrapper.h

FORMS    += mainwindow.ui

RESOURCES +=
