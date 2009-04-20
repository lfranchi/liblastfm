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
VERSION = 0.2
INSTALLS = target
target.path = /lib

# us at Last.fm want these but you prolly don't
macx*:SOURCES -= core/mac/Growl.cpp core/mac/Applescript.cpp

# allow g++ to super optimise us
#release:SOURCES = _all.cpp

unix:QMAKE_CXXFLAGS_RELEASE += -fvisibility-inlines-hidden -fvisibility=hidden
