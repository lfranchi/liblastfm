include(core/core.pro)
include(ws/ws.pro)
include(types/types.pro)
include(fingerprint/fingerprint.pro)
include(radio/radio.pro)
include(scrobble/scrobble.pro)
include(_files.qmake)

TEMPLATE = lib
TARGET = lastfm
QT = core network xml sql
VERSION = 0.2.1
INSTALLS = target
target.path = /lib

# us at Last.fm want these but you prolly don't
macx*:SOURCES -= core/mac/Growl.cpp core/mac/Applescript.cpp

unix{
    QMAKE_CXXFLAGS_RELEASE -= -O2 -Os
    QMAKE_CXXFLAGS_RELEASE += -fvisibility-inlines-hidden -fvisibility=hidden -O3 -fomit-frame-pointer
    linux*{
        QMAKE_CXXFLAGS_RELEASE += -freorder-blocks -fno-reorder-functions -w
    }    
    release:QMAKE_POST_LINK=strip $(DESTDIR)/$(TARGET)
}
