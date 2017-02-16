
VERSION_MAJOR = 0
VERSION_MINOR = 6
VERSION_PATCH = 0
VERSION = $$VERSION_MAJOR'.'$$VERSION_MINOR'.'$$VERSION_PATCH

DESTDIR = $$OUT_PWD/../bin

# подключить библиотеки и *.h файлы
CONFIG += depend_includepath
unix: CONFIG += debug_and_release debug_and_release_target

# выведем сообщение компилятора в каком режиме собираем (для проверки)
release:message(Building in release mode.)
debug:message(Building in debug mode.)

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
  LIBS += -lextensions -lextensions-gui -lqwt
} else:CONFIG(debug, debug|release) {
  LIBS += -lextensionsd -lextensions-guid
  unix {
    LIBS += -lqwt
  } else:win32 {
    LIBS += -lqwtd
  }
}

INCLUDEPATH += $${DEP_PATH_HEADERS}/extensions
INCLUDEPATH += $${DEP_PATH_HEADERS}/extensions-gui
# qwt
INCLUDEPATH += $${DEP_PATH_HEADERS}
INCLUDEPATH += $${DEP_PATH_HEADERS}/qwt

INCLUDEPATH += ../ganttmodel/memoryplanningwidget
INCLUDEPATH += ../ganttmodel/memoryplanningwidget/memoryline
INCLUDEPATH += ../ganttmodel/memoryplanningwidget/memorygrid
