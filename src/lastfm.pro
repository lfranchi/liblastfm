TEMPLATE = lib
TARGET = lastfm
QT = core network xml
VERSION = 0.3
INSTALLS = target
include( _files.qmake )

# ws configuration
win32:DEFINES += _ATL_DLL 
win32:LIBS += winhttp.lib wbemuuid.lib
macx*:LIBS += -framework SystemConfiguration

target.path = /lib