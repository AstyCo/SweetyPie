#-------------------------------------------------
#
# Project created by QtCreator 2017-02-20T14:44:17
#
#-------------------------------------------------

TARGET = gantt-static-lib
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += GANTT_STATIC_LIBRARY

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../version.pri)
include(../gantt-static-lib-include.pri)

SOURCES += \
    ganttwidget.cpp \
    extensions/dtline.cpp \
    extensions/intervalslider.cpp \
    extensions/intervalunion.cpp \
    extensions/myutcdatetimeinterval.cpp \
    extensions/qrectunion.cpp \
    extensions/timespan_extension.cpp \
    gantt/factory/ganttfactory.cpp \
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
    gantt/plot/scene_objects/gantttextgraphicsobject.cpp \
    gantt/plot/ganttdtline.cpp \
    gantt/plot/ganttgraphicsview.cpp \
    gantt/plot/ganttscene.cpp \
    gantt/plot/hfitscene.cpp \
    gantt/private_extensions/mousepresshelper.cpp \
    gantt/ganttintervalslider.cpp \
    gantt/info/treewalker.cpp

HEADERS += \
    ganttwidget.h \
    extensions/dtline.h \
    extensions/intervalslider.h \
    extensions/intervalunion.h \
    extensions/myutcdatetimeinterval.h \
    extensions/qrectunion.h \
    extensions/timespan_extension.h \
    gantt/factory/ganttfactory.h \
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
    gantt/plot/scene_objects/gantttextgraphicsobject.h \
    gantt/plot/ganttdtline.h \
    gantt/plot/ganttgraphicsview.h \
    gantt/plot/ganttscene.h \
    gantt/plot/hfitscene.h \
    gantt/private_extensions/gantt-lib_global_values.h \
    gantt/private_extensions/idtinterval.h \
    gantt/private_extensions/mousepresshelper.h \
    gantt/ganttintervalslider.h \
    gantt-static-lib-global.h \
    gantt/info/treewalker.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    ganttwidget.ui \
    gantt/player/ganttplayercontrol.ui

RESOURCES += \
    images.qrc

include(build-config.pri)
