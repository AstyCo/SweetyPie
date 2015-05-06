#-------------------------------------------------
#
# Project created by QtCreator 2014-12-24T12:45:35
#
#-------------------------------------------------

QT       += gui

unix:{
VERSION = 0.1.0
}
Debug:TARGET = GanttModeld
Release:TARGET = GanttModel

TEMPLATE = lib



DEFINES += GANTTMODEL_LIBRARY

SOURCES += \
    ganttmodel.cpp \
    ganttitem.cpp \
    ganttview.cpp \
    ganttgraphicsview.cpp \
    gantttreeview.cpp \
#    ganttitemstage.cpp \
    ganttgraphicsscene.cpp \
    ganttgraphicsitem.cpp \
#    ganttgraphicsitemstage.cpp \
#    ganttdatetimegrid.cpp \
    ganttgraphicsheader.cpp \
    ganttgraphicsheaderrectitem.cpp \
    ganttdatetimedelegate.cpp \
    ganttgraphicsheaderitem.cpp \
    ganttplayer.cpp

HEADERS += \
    ganttmodel.h \
    ganttitem.h \
    ganttview.h \
    ganttgraphicsview.h \
    gantttreeview.h \
#    ganttitemstage.h \
    ganttgraphicsscene.h \
    ganttgraphicsitem.h \
#    ganttgraphicsitemstage.h \
#    ganttdatetimegrid.h \
    ganttgraphicsheader.h \
    ganttgraphicsheaderrectitem.h \
    ganttmodel_global.h \
    ganttdatetimedelegate.h \
    ganttgraphicsheaderitem.h \
    ganttplayer.h


RESOURCES += \
    images.qrc
