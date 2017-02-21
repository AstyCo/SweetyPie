#-------------------------------------------------
#
# Project created by QtCreator 2015-07-22T10:51:50
#
#-------------------------------------------------

#include(qwtconfig.pri )
#include(qwtbuild.pri )
#include(qwtfunctions.pri )

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChartWidget
TEMPLATE = app

include(../gantt-lib-include.pri)
include(../demo-lib-include.pri)

SOURCES += main.cpp\
        mainwindow.cpp


HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

