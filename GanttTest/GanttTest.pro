#-------------------------------------------------
#
# Project created by QtCreator 2015-02-09T11:19:13
#
#-------------------------------------------------

QT       += core gui

TARGET = GanttTest
TEMPLATE = app

Debug:LIBS += -L"../ganttmodel/debug" -lganttmodeld0
Release:LIBS += -L"../ganttmodel/release" -lganttmodel0

INCLUDEPATH += "../ganttmodel"
INCLUDEPATH += "../../../lib/include"
INCLUDEPATH += "../../../src/src_ballistics_lib/ballistics_lib"

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
