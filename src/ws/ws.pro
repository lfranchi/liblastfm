TEMPLATE = lib
QT = core network xml
DEFINES += _WS_DLLEXPORT
win32:DEFINES += _ATL_DLL 
win32:LIBS += winhttp.lib wbemuuid.lib
macx*:LIBS += -framework SystemConfiguration
headers.files = WsAccessManager.h WsError.h WsKeys.h WsRequestBuilder.h WsReply.h WsDomElement.h
include( _files.qmake )
