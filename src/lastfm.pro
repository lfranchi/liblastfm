TEMPLATE = lib
TARGET = lastfm
QT = core network xml

# we don't want windows lib files to have the major version in the name
#
!win32 {
	VERSION = 0.3
}

INSTALLS = target
include( _files.qmake )

# ws configuration
win32:DEFINES += LASTFM_OHAI_QMAKE _ATL_DLL 
win32:LIBS += winhttp.lib wbemuuid.lib
macx*:LIBS += -framework SystemConfiguration

target.path = /lib