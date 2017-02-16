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
    extensions/dtline.cpp \
    extensions/intervalslider.cpp \
    extensions/timespan_extension.cpp \
    gantt/info/ganttinfoitem.cpp \
    gantt/info/ganttinfoleaf.cpp \
    gantt/info/ganttinfonode.cpp \
    gantt/info/ganttinfotree.cpp \
    gantt/player/ganttplayercontrol.cpp \
    gantt/player/ganttplayersettings.cpp \
    gantt/player/playercontrolbutton.cpp \
    gantt/plot/scene_objects/ganttcalcgraphicsobject.cpp \
    gantt/plot/scene_objects/ganttcurrentdtslider.cpp \
    gantt/plot/scene_objects/ganttdtcrossobject.cpp \
    gantt/plot/scene_objects/ganttgraphicsobject.cpp \
    gantt/plot/scene_objects/gantthovergraphicsobject.cpp \
    gantt/plot/scene_objects/ganttintervalgraphicsobject.cpp \
    gantt/plot/ganttdtline.cpp \
    gantt/plot/ganttgraphicsview.cpp \
    gantt/plot/ganttscene.cpp \
    gantt/plot/hfitscene.cpp \
    gantt/ganttintervalslider.cpp \
    ganttwidget.cpp \
    dtintervalwidget/dtintervalwidget.cpp \
    extensions/qrectunion.cpp \
    gantt/private_extensions/mousepresshelper.cpp \
    extensions/intervalunion.cpp \
    extensions/myutcdatetimeinterval.cpp \
    gantt/builder/abstractbuilder.cpp \
    gantt/builder/ganttbuilder.cpp \
    gantt/builder/imodelwrapper.cpp


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
    extensions/dtline.h \
    extensions/timespan_extension.h \
    gantt/info/ganttinfoitem.h \
    gantt/info/ganttinfoleaf.h \
    gantt/info/ganttinfonode.h \
    gantt/info/ganttinfotree.h \
    gantt/player/ganttplayercontrol.h \
    gantt/player/ganttplayersettings.h \
    gantt/player/playercontrolbutton.h \
    gantt/plot/scene_objects/ganttcalcgraphicsobject.h \
    gantt/plot/scene_objects/ganttcurrentdtslider.h \
    gantt/plot/scene_objects/ganttdtcrossobject.h \
    gantt/plot/scene_objects/ganttgraphicsobject.h \
    gantt/plot/scene_objects/gantthovergraphicsobject.h \
    gantt/plot/scene_objects/ganttintervalgraphicsobject.h \
    gantt/plot/ganttdtline.h \
    gantt/plot/ganttgraphicsview.h \
    gantt/plot/ganttscene.h \
    gantt/plot/hfitscene.h \
    gantt/ganttintervalslider.h \
    extensions/intervalslider.h \
    ganttwidget.h \
    dtintervalwidget/dtintervalwidget.h \
    extensions/qrectunion.h \
    gantt/private_extensions/idtinterval.h \
    gantt/private_extensions/gantt-lib_global_values.h \
    gantt/private_extensions/mousepresshelper.h \
    extensions/intervalunion.h \
    extensions/myutcdatetimeinterval.h \
    gantt/builder/abstractbuilder.h \
    gantt/builder/ganttbuilder.h \
    gantt/builder/imodelwrapper.h


RESOURCES += \
    images.qrc

FORMS += \
    curvedetailsgroupbox.ui \
    chartsgroupwidget.ui \
    chartsettingsdlg.ui \
    memoryplanningwidget/memoryplanningwidget.ui \
    chartxywidget.ui \
    chartintervalselectionwidget.ui \
    gantt/player/ganttplayercontrol.ui \
    ganttwidget.ui \
    dtintervalwidget/dtintervalwidget.ui

DISTFILES += \
    ../gantt-lib-include.pri \
    build_config.pri

include(../build_config.pri)
