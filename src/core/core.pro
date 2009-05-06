TARGET = core
TEMPLATE = lib
DEFINES += _CORE_DLLEXPORT
QT = core xml
headers.files = UrlBuilder.h XmlQuery.h ../global.h misc.h
include( _files.qmake )
