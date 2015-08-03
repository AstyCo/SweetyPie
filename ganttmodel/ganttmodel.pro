#-------------------------------------------------
#
# Project created by QtCreator 2014-12-24T12:45:35
#
#-------------------------------------------------

QT       += core gui


VERSION = 0.2.0

TEMPLATE = lib

DEFINES += QWT_DLL

unix:{
    # убираем не очевидную директиву
    CONFIG -= debug_and_release
    # проверяем, если debug сборка, то убираем флаг release, который устанавливается всегда
    CONFIG( debug, debug|release )  {
      CONFIG -= release
    }
    else {
    # если release, на всякий случаем убираем debug и добавляем release
      CONFIG -= debug
      CONFIG += release
    }

    # теперь точно поддерживается проверка на release и debug флаги
    # все дополнительные файлы раскидаем по директориям
    release:{
            DESTDIR = release
            OBJECTS_DIR=$(DESTDIR)
            MOC_DIR = release/.moc
            RCC_DIR = release/.qrc
            UI_DIR = release/.ui
        } else {
            DESTDIR = debug
            OBJECTS_DIR=$(DESTDIR)
            MOC_DIR = debug/.moc
            RCC_DIR = debug/.qrc
            UI_DIR = debug/.ui
        }
}
 # выведем сообщение компилятора в каком режиме собираем (для проверки)
 release:message(Building in release mode.)
 debug:message(Building in debug mode.)

# подключить библиотеки и *.h файлы
win32:{

    Release:TARGET = ganttmodel
    Debug:TARGET = ganttmodeld


    Debug:LIBS += -L"../../../lib/win32"  -lqwtd
    Release:LIBS += -L"../../../lib/win32"  -lqwt

    Debug:LIBS += -L"../../../src/ballistics/ballistics_lib/debug" -lballisticsd0
    Release:LIBS += -L"../../../src/ballistics/ballistics_lib/release" -lballistics0
}

unix:{
    release:TARGET = ganttmodel
    debug:TARGET = ganttmodeld

    debug:LIBS += -L"../../../lib/msvs5"  -lqwt
    release:LIBS += -L"../../../lib/msvs5"  -lqwt

    debug:LIBS += -lballisticsd0
    release:LIBS += -lballistics0
}

INCLUDEPATH += "../../../lib/include"
INCLUDEPATH += "../../../src/src_ballistics_lib/ballistics_lib"


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
