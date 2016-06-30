#-------------------------------------------------
#
# Project created by QtCreator 2014-12-24T12:45:35
#
#-------------------------------------------------

QT       += core gui


VERSION = 0.5.3

TEMPLATE = lib

DEFINES += QWT_DLL

#DLLDESTDIR = $$OUT_PWD/
TARGET = ganttmodel

DESTDIR = $$OUT_PWD/../bin
DLLDESTDIR = $$DESTDIR

# пути к зависимым библиотекам
unix {
  DEP_PATH_LIBS = /usr/lib64
  DEP_PATH_HEADERS = /usr/include
} else:win32 {
  DEP_PATH_LIBS = C:/lib64
  DEP_PATH_HEADERS = C:/include
}

LIBS += -L$${DEP_PATH_LIBS}

CONFIG(release, debug|release) {
  LIBS += -lextensions -lqwt
} else:CONFIG(debug, debug|release) {
  LIBS += -lextensionsd -lqwt
  unix {
    LIBS += -lqwt
  } else:win32 {
    LIBS += -lqwtd
  }
}


INCLUDEPATH += $${DEP_PATH_HEADERS}/extensions
DEPENDPATH += $${DEP_PATH_HEADERS}/extensions

# qwt
INCLUDEPATH += $${DEP_PATH_HEADERS}
INCLUDEPATH += $${DEP_PATH_HEADERS}/qwt

DEFINES += GANTTMODEL_LIBRARY

SOURCES += \
    ganttmodel.cpp \
    ganttitem.cpp \
    ganttview.cpp \
    ganttgraphicsview.cpp \
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
    chartsgroupwidget.cpp \
    chartactionstoolbar.cpp \
    plotkeyeventhandler.cpp \
    chartsettingsdlg.cpp \
    chartsettings.cpp \
    plotnavigator.cpp \
    intervalselectionmodel.cpp

HEADERS += \
    ganttmodel.h \
    ganttitem.h \
    ganttview.h \
    ganttgraphicsview.h \
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
    chartsgroupwidget.h \
    chartactionstoolbar.h \
    plotkeyeventhandler.h \
    chartsettingsdlg.h \
    chartsettings.h \
    plotnavigator.h \
    intervalselectionmodel.h


RESOURCES += \
    images.qrc

FORMS += \
    chartwidget.ui \
    curvedetailsgroupbox.ui \
    chartsgroupwidget.ui \
    chartsettingsdlg.ui

include(build_config.pri)
