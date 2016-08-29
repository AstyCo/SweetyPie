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
    gantt/ganttintervalsliderzoom.cpp \
    gantt/ganttplayersettings.cpp \
    gantt/main.cpp \
    gantt/playercontrolbutton.cpp \
    gantt/qgraphicsrectitemwithtext.cpp \
    gantt/ganttwidget.cpp \
    gantt/gantttreeview.cpp \
    gantt/gantttreemodel.cpp \
    gantt/gantttreedelegate.cpp \
    gantt/ganttscene.cpp \
    gantt/ganttplayercontrol.cpp \
    gantt/ganttintervalslider.cpp \
    gantt/ganttinfonode.cpp \
    gantt/ganttinfoleaf.cpp \
    gantt/ganttinfoitem.cpp \
    gantt/ganttheaderview.cpp \
    gantt/ganttheader.cpp \
    gantt/ganttgraphicsview.cpp \
    appeventfilter.cpp \
    gantt/ganttcalcgraphicsobject.cpp \
    gantt/ganttgraphicsobject.cpp \
    gantt/ganttcurrentdtslider.cpp


HEADERS += \
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
    gantt/ganttintervalsliderzoom.h \
    gantt/ganttplayersettings.h \
    gantt/playercontrolbutton.h \
    gantt/qgraphicsrectitemwithtext.h \
    gantt/ganttwidget.h \
    gantt/gantttreeview.h \
    gantt/gantttreemodel.h \
    gantt/gantttreedelegate.h \
    gantt/ganttscene.h \
    gantt/ganttplayercontrol.h \
    gantt/ganttintervalslider.h \
    gantt/ganttinfonode.h \
    gantt/ganttinfoleaf.h \
    gantt/ganttinfoitem.h \
    gantt/ganttheaderview.h \
    gantt/ganttheader.h \
    gantt/ganttgraphicsview.h \
    gantt/ganttglobalvalues.h \
    ganttproject_global.h \
    appeventfilter.h \
    gantt/ganttgraphicsobject.h \
    gantt/ganttcalcgraphicsobject.h \
    gantt/ganttcurrentdtslider.h


RESOURCES += \
    images.qrc

FORMS += \
    chartwidget.ui \
    curvedetailsgroupbox.ui \
    chartsgroupwidget.ui \
    chartsettingsdlg.ui \
    memoryplanningwidget/memoryplanningwidget.ui \
    gantt/ganttintervalsliderzoom.ui \
    gantt/ganttplayersettings.ui \
    gantt/ganttwidget.ui \
    gantt/mygantt_playercontrol.ui

include(build_config.pri)

CODECFORTR = UTF-8

TRANSLATIONS += \
    locale/file_codec_locale_ru.ts
