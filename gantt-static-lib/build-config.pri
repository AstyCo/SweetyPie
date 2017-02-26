# Файл настроек проекта статической библиотеки, являющейся подпроектом динамической

MAIN_PROJECT = ganttmodel


# Исходный префикс для абсолютного пути установки библиотеки
unix{
  INSTALL_PREFIX = /usr
  # Собирать сразу две версии (Debug и Release)
  CONFIG += debug_and_release

} else:win32 {
  INSTALL_PREFIX = C:/
}

# Полный путь установки заголовочных файлов
INSTALL_HEADERS_PATH = $${INSTALL_PREFIX}/include/$${MAIN_PROJECT}

# Создать правило установки заголовочных файлов с относительными директориями
CONFIG(debug, debug|release) {
  INSTALL_HEADERS = $$HEADERS
  for(header, INSTALL_HEADERS) {
    dir = $${dirname(header)}
    path = $${INSTALL_HEADERS_PATH}/$${dir}
    eval(headers_$${dir}.files += $$header)
    eval(headers_$${dir}.path = $$path)
    eval(INSTALLS *= headers_$${dir})
  }
}
