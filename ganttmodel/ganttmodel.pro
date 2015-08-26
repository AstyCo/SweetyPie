#-------------------------------------------------
#
# Project created by QtCreator 2014-12-24T12:45:35
#
#-------------------------------------------------

QT       += core gui


VERSION = 0.4.2

TEMPLATE = lib

DEFINES += QWT_DLL

#DLLDESTDIR = $$OUT_PWD/
TARGET = ganttmodel

# подключить библиотеки и *.h файлы
win32:{

    #DESTDIR = ../../../lib/win32

    Debug:LIBS += -L"../../../lib/win32"  -lqwtd
    Release:LIBS += -L"../../../lib/win32"  -lqwt

    Debug:LIBS += -L"../../src_ballistics_lib/ballistics_lib" -lballisticsd0
    Release:LIBS += -L"../../src_ballistics_lib/ballistics_lib" -lballistics0
}

unix:{

    #DESTDIR  = ../../../lib/msvs5

    debug:LIBS += -L"../../../lib/msvs5"  -lqwt
    release:LIBS += -L"../../../lib/msvs5"  -lqwt

    debug:LIBS += -L"/usr/lib64" -lballisticsd
    release:LIBS += -L"/usr/lib64" -lballistics
}

INCLUDEPATH += "../../../lib/include"
INCLUDEPATH += "../../../src/src_ballistics_lib/ballistics_lib"
unix:INCLUDEPATH += "/usr/include/ballistics"


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
    ganttplayer.cpp \
    chartwidget.cpp \
    curvedetailsgroupbox.cpp \
    intervalslider.cpp \
    plotinterval.cpp \
    plotmagnifierx.cpp \
    plotkeyeventhander.cpp

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
    ganttplayer.h \
    chartwidget.h \
    curvedetailsgroupbox.h \
    intervalslider.h \
    plotinterval.h \
    plotmagnifierx.h \
    plotkeyeventhander.h


RESOURCES += \
    images.qrc

FORMS += \
    chartwidget.ui \
    curvedetailsgroupbox.ui

include(build_config.pri)
