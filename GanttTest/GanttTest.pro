#-------------------------------------------------
#
# Project created by QtCreator 2015-02-09T11:19:13
#
#-------------------------------------------------

QT       += core gui

TARGET = GanttTest
TEMPLATE = app

Debug:LIBS += -L"../ganttmodel/debug" -lGanttModeld
Release:LIBS += -L"../ganttmodel/release" -lGanttModel

INCLUDEPATH += "../ganttmodel"

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
