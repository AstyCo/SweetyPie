#-------------------------------------------------
#
# Project created by QtCreator 2014-12-24T12:45:35
#
#-------------------------------------------------

QT       += core gui

TEMPLATE = lib

DEFINES += QWT_DLL
DEFINES += GANTTMODEL_LIBRARY

TARGET = ganttmodel

include(../version.pri)
include(../gantt-lib-include.pri)


SOURCES += \
    curvedetailsgroupbox.cpp \
    plotinterval.cpp \
    chartsgroupwidget.cpp \
    chartactionstoolbar.cpp \
    plotkeyeventhandler.cpp \
    chartsettingsdlg.cpp \
    chartsettings.cpp \
    plotnavigator.cpp \
    memoryplanningwidget/memoryview.cpp \
    memoryplanningwidget/memoryplanningwidget.cpp \
    memoryplanningwidget/memorygrid/mgrid_interactiveunit.cpp \
    memoryplanningwidget/memorygrid/mgrid_item.cpp \
    memoryplanningwidget/memorygrid/mgrid_scene.cpp \
    memoryplanningwidget/memorygrid/mgrid_unit.cpp \
    memoryplanningwidget/memoryline/mline_abstractpart.cpp \
    memoryplanningwidget/memoryline/mline_graphicspart.cpp \
    memoryplanningwidget/memoryline/mline_picker.cpp \
    memoryplanningwidget/memoryline/mline_scene.cpp \
    memoryplanningwidget/kamemorylegend.cpp \
    charttimexywidget.cpp \
    chartxywidget.cpp \
    chartintervalselectionwidget.cpp \
    chartintervalselector.cpp \
    dtintervalwidget/dtintervalwidget.cpp


HEADERS += \
    curvedetailsgroupbox.h \
    plotinterval.h \
    chartsgroupwidget.h \
    chartactionstoolbar.h \
    plotkeyeventhandler.h \
    chartsettingsdlg.h \
    chartsettings.h \
    plotnavigator.h \
    memoryplanningwidget/memoryline/mline_abstractpart.h \
    memoryplanningwidget/memoryline/mline_picker.h \
    memoryplanningwidget/memoryline/mline_graphicspart.h \
    memoryplanningwidget/memorygrid/mgrid_interactiveunit.h \
    memoryplanningwidget/memorygrid/mgrid_item.h \
    memoryplanningwidget/memorygrid/mgrid_scene.h \
    memoryplanningwidget/memorygrid/mgrid_unit.h \
    memoryplanningwidget/memoryplanningwidget.h \
    memoryplanningwidget/memoryview.h \
    memoryplanningwidget/memoryline/mline_scene.h \
    memoryplanningwidget/memory_globalvalues.h \
    memoryplanningwidget/kamemorylegend.h \
    ganttproject_global.h \
    charttimexywidget.h \
    chartxywidget.h \
    chartintervalselectionwidget.h \
    chartintervalselector.h \
    dtintervalwidget/dtintervalwidget.h


RESOURCES += \
    images.qrc

FORMS += \
    curvedetailsgroupbox.ui \
    chartsgroupwidget.ui \
    chartsettingsdlg.ui \
    memoryplanningwidget/memoryplanningwidget.ui \
    chartxywidget.ui \
    chartintervalselectionwidget.ui \
    dtintervalwidget/dtintervalwidget.ui

include(../build_config.pri)
