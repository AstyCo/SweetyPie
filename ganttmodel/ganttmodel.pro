#-------------------------------------------------
#
# Project created by QtCreator 2014-12-24T12:45:35
#
#-------------------------------------------------

QT       += core gui



VERSION = 0.5.6

TEMPLATE = lib

DEFINES += QWT_DLL

TARGET = ganttmodel

DESTDIR = $$OUT_PWD/../bin

#DLLDESTDIR = $$DESTDIR

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
  LIBS += -lextensionsd
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

INCLUDEPATH += "$$PWD/../ganttmodel"
INCLUDEPATH += "$$PWD/../ganttmodel/memoryplanningwidget"
INCLUDEPATH += "$$PWD/../ganttmodel/memoryplanningwidget/memoryline"
INCLUDEPATH += "$$PWD/../ganttmodel/memoryplanningwidget/memorygrid"
INCLUDEPATH += "$$PWD/../ganttmodel/gantt"


DEFINES += GANTTMODEL_LIBRARY

SOURCES += \
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
    memoryplanningwidget/memoryview.cpp \
    memoryplanningwidget/memoryplanningwidget.cpp \
    memoryplanningwidget/kamemory.cpp \
    memoryplanningwidget/kamemorypart.cpp \
    memoryplanningwidget/memorygrid/mgrid_interactiveunit.cpp \
    memoryplanningwidget/memorygrid/mgrid_item.cpp \
    memoryplanningwidget/memorygrid/mgrid_scene.cpp \
    memoryplanningwidget/memorygrid/mgrid_unit.cpp \
    memoryplanningwidget/memoryline/mline_abstractpart.cpp \
    memoryplanningwidget/memoryline/mline_graphicspart.cpp \
    memoryplanningwidget/memoryline/mline_picker.cpp \
    memoryplanningwidget/memoryline/mline_scene.cpp \
    memoryplanningwidget/kamemorylegend.cpp \
    gantt/ganttview.cpp \
    gantt/ganttplayer.cpp \
    gantt/ganttmodel.cpp \
    gantt/ganttitem.cpp \
    gantt/ganttgraphicsview.cpp \
    gantt/ganttgraphicsscene.cpp \
    gantt/ganttgraphicsitem.cpp \
    gantt/ganttgraphicsheaderrectitem.cpp \
    gantt/ganttgraphicsheaderitem.cpp \
    gantt/ganttgraphicsheader.cpp \
    gantt/ganttdatetimedelegate.cpp


HEADERS += \
#    ganttitemstage.h \
#    ganttgraphicsitemstage.h \
#    ganttdatetimegrid.h \
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
    memoryplanningwidget/kamemory.h \
    memoryplanningwidget/kamemorypart.h \
    memoryplanningwidget/memory_globalvalues.h \
    memoryplanningwidget/kamemorylegend.h \
    gantt/ganttview.h \
    gantt/ganttdatetimedelegate.h \
    gantt/ganttgraphicsheader.h \
    gantt/ganttgraphicsheaderitem.h \
    gantt/ganttgraphicsheaderrectitem.h \
    gantt/ganttgraphicsitem.h \
    gantt/ganttgraphicsscene.h \
    gantt/ganttgraphicsview.h \
    gantt/ganttitem.h \
    gantt/ganttmodel.h \
    gantt/ganttplayer.h \
    ganttproject_global.h


RESOURCES += \
    images.qrc

FORMS += \
    chartwidget.ui \
    curvedetailsgroupbox.ui \
    chartsgroupwidget.ui \
    chartsettingsdlg.ui \
    memoryplanningwidget/memoryplanningwidget.ui

include(build_config.pri)

CODECFORTR = UTF-8

TRANSLATIONS += \
    locale/file_codec_locale_ru.ts
