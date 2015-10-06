#-------------------------------------------------
#
# Project created by QtCreator 2014-12-24T12:45:35
#
#-------------------------------------------------

QT       += core gui


VERSION = 0.4.4

TEMPLATE = lib

DEFINES += QWT_DLL

#DLLDESTDIR = $$OUT_PWD/
TARGET = ganttmodel

# пути к зависимым библиотекам
unix {
  DEP_PATH_LIBS = /usr/lib64
  DEP_PATH_HEADERS = /usr/include
} else:win32 {
  DEP_PATH_LIBS = C:/lib64
  DEP_PATH_HEADERS = C:/include
}

LIBS += -L$${DEP_PATH_LIBS}

win32 {
  CONFIG(release, debug|release) {
    LIBS += -lballistics -lqwt
  } else:CONFIG(debug, debug|release) {
    LIBS += -lballisticsd -lqwtd
  }
} else:unix {
  CONFIG(release, debug|release) {
    LIBS += -lballistics -lqwt
  } else:CONFIG(debug, debug|release) {
    LIBS += -lballisticsd -lqwt
  }
}

# ballistics lib
INCLUDEPATH += $${DEP_PATH_HEADERS}/ballistics
DEPENDPATH += $${DEP_PATH_HEADERS}/ballistics
# qwt
INCLUDEPATH += $${DEP_PATH_HEADERS}/qwt

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
    plotkeyeventhander.cpp \
    verticalscrollarea.cpp \
    chartsgroupwidget.cpp \
    chartactionstoolbar.cpp

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
    plotkeyeventhander.h \
    verticalscrollarea.h \
    chartsgroupwidget.h \
    chartactionstoolbar.h


RESOURCES += \
    images.qrc

FORMS += \
    chartwidget.ui \
    curvedetailsgroupbox.ui \
    chartsgroupwidget.ui

include(build_config.pri)
