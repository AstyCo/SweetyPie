LIBS = -L$$DESTDIR $$LIBS

CONFIG(release, debug|release) {
  LIBS += -lganttmodel
} else:CONFIG(debug, debug|release) {
  LIBS += -lganttmodeld
}
