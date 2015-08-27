#-------------------------------------------------
#
# Project created by QtCreator 2015-02-09T11:19:13
#
#-------------------------------------------------

QT       += core gui

TARGET = GanttTest
TEMPLATE = app


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

Debug:LIBS += -L"../ganttmodel" -lganttmodeld
Release:LIBS += -L"../ganttmodel" -lganttmodel

Debug:LIBS += -L"C:/lib64" -lballisticsd
Release:LIBS += -L"C:/lib64" -lballistics
}
unix{

debug:LIBS += -L"../ganttmodel" -lganttmodeld
release:LIBS += -L"../ganttmodel" -lganttmodel

debug:LIBS += -L"/usr/lib64" -lballisticsd
release:LIBS += -L"/usr/lib64" -lballistics

}

INCLUDEPATH += "../ganttmodel"
unix:INCLUDEPATH += "/usr/include/ballistics"
win32:INCLUDEPATH += "C:/include/ballistics"
unix:INCLUDEPATH += "/usr/include/qwt"
win32:INCLUDEPATH += "C:/include/qwt"


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
