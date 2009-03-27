TARGET = core
TEMPLATE = lib
DEFINES += _CORE_DLLEXPORT
QT = core xml
macx*:LIBS += -framework Carbon # for mac/AppleScript
win32:LIBS += shell32.lib
headers.files = UrlBuilder.h
include( _files.qmake )
