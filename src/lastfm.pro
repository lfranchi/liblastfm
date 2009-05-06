TEMPLATE = lib
TARGET = lastfm
QT = core network xml
VERSION = 0.3
INSTALLS = target
DEFINES += LASTFM_OHAI_QMAKE
include( _files.qmake )

SOURCES -= $$system( $$ROOT_DIR/admin/findsrc cpp fingerprint )
HEADERS -= $$system( $$ROOT_DIR/admin/findsrc h fingerprint )

unix{
    QMAKE_CXXFLAGS_RELEASE -= -O2 -Os
    QMAKE_CXXFLAGS_RELEASE += -fvisibility-inlines-hidden -fvisibility=hidden -O3 -fomit-frame-pointer
    linux*{
        QMAKE_CXXFLAGS_RELEASE += -freorder-blocks -fno-reorder-functions -w
    }    
    release:QMAKE_POST_LINK=strip $(DESTDIR)/$(TARGET)
}

# ws configuration
win32:DEFINES += _ATL_DLL 
win32:LIBS += winhttp.lib wbemuuid.lib
macx*:LIBS += -framework SystemConfiguration

target.path = /lib