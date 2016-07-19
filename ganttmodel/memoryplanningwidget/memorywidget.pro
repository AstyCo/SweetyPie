#-------------------------------------------------
#
# Project created by QtCreator 2016-07-07T10:38:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = memorywidget
TEMPLATE = app


SOURCES += main.cpp\
        memorywidget.cpp \
    memoryview.cpp \
    memoryitem.cpp \
    memoryscene.cpp \
    appeventfilter.cpp \
    labelitem.cpp \
    memoryitempresentation.cpp \
    memoryunit.cpp \
    memorystatus.cpp \
    memoryinteractiveunit.cpp \
    memoryplanningwidget.cpp

HEADERS  += \
    globalvalues.hpp \
    labelitem.hpp \
    memoryitempresentation.hpp \
    memoryscene.hpp \
    memoryview.hpp \
    memorywidget.hpp \
    memoryitem.hpp \
    appeventfilter.hpp \
    memoryunit.hpp \
    memorystatus.hpp \
    memoryinteractiveunit.hpp \
    memoryplanningwidget.h

FORMS    += \
    form.ui \
    memoryplanningwidget.ui
