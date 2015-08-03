#-------------------------------------------------
#
# Project created by QtCreator 2015-07-22T10:51:50
#
#-------------------------------------------------

#include(qwtconfig.pri )
#include(qwtbuild.pri )
#include(qwtfunctions.pri )


QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChartWidget
TEMPLATE = app

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
win32{

Debug:LIBS += -L"../ganttmodel/debug" -lganttmodeld0
Release:LIBS += -L"../ganttmodel/release" -lganttmodel0

Debug:LIBS += -L"../../../src/ballistics/ballistics_lib/debug" -lballisticsd0
Release:LIBS += -L"../../../src/ballistics/ballistics_lib/release" -lballistics0


}
unix{

Debug:LIBS += -L"../ganttmodel/debug" -lganttmodeld
Release:LIBS += -L"../ganttmodel/release" -lganttmodel


debug:LIBS += -lballisticsd0
release:LIBS += -lballistics0
}

INCLUDEPATH += "../ganttmodel"
INCLUDEPATH += "../../../lib/include"
INCLUDEPATH += "../../../src/src_ballistics_lib/ballistics_lib"


SOURCES += main.cpp\
        mainwindow.cpp \
    chartsettingsdlg.cpp \
    verticalscrollarea.cpp


HEADERS  += mainwindow.h \
    chartsettingsdlg.h \
    verticalscrollarea.h

FORMS    += mainwindow.ui \
    chartsettingsdlg.ui

