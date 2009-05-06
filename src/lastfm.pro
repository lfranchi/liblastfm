TEMPLATE = lib
TARGET = lastfm
QT = core network xml
VERSION = 0.3
INSTALLS = target
include( _files.qmake )

SOURCES -= $$system( $$ROOT_DIR/admin/findsrc cpp fingerprint )
HEADERS -= $$system( $$ROOT_DIR/admin/findsrc h fingerprint )

# ws configuration
win32:DEFINES += _ATL_DLL 
win32:LIBS += winhttp.lib wbemuuid.lib
macx*:LIBS += -framework SystemConfiguration

target.path = /lib