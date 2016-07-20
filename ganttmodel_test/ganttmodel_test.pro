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

#DEFINES += QWT_DLL

DESTDIR = $$OUT_PWD/../bin

unix:{    
    CONFIG += debug_and_release debug_and_release_target
}
 # выведем сообщение компилятора в каком режиме собираем (для проверки)
 release:message(Building in release mode.)
 debug:message(Building in debug mode.)


# подключить библиотеки и *.h файлы
CONFIG += depend_includepath

INCLUDEPATH += "$$PWD/../ganttmodel"
INCLUDEPATH += "$$PWD/../ganttmodel/memoryplanningwidget"
INCLUDEPATH += "$$PWD/../ganttmodel/memoryplanningwidget/memoryline"
INCLUDEPATH += "$$PWD/../ganttmodel/memoryplanningwidget/memorygrid"

message(DESTDIR: $$DESTDIR)
LIBS += -L"$${DESTDIR}"
CONFIG(release, debug|release) {
  LIBS += -lganttmodel
} else:CONFIG(debug, debug|release) {
  LIBS += -lganttmodeld
}

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
INCLUDEPATH += $${DEP_PATH_HEADERS}


SOURCES += main.cpp\
        mainwindow.cpp


HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

