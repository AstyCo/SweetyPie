include(gantt-static-lib-include.pri)

message(added library: "$$DESTDIR/libgantt-static-lib.a")
LIBS += "$$DESTDIR/libgantt-static-lib.a" # link static library

message(added to INCLUDEPATH: $$_PRO_FILE_PWD_/../gantt-static-lib)
INCLUDEPATH += $$_PRO_FILE_PWD_/../gantt-static-lib # include headers


LIBS += -L$${DEP_PATH_LIBS}

CONFIG(release, debug|release) {
  LIBS += -lqwt
} else:CONFIG(debug, debug|release) {
  unix {
    LIBS += -lqwt
  } else:win32 {
    LIBS += -lqwtd
  }
}

# qwt
INCLUDEPATH += $${DEP_PATH_HEADERS}/qwt

INCLUDEPATH += ../ganttmodel/memoryplanningwidget
INCLUDEPATH += ../ganttmodel/memoryplanningwidget/memoryline
INCLUDEPATH += ../ganttmodel/memoryplanningwidget/memorygrid
