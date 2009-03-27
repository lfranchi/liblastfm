include(core/core.pro)
include(ws/ws.pro)
include(types/types.pro)
include(fingerprint/fingerprint.pro)
include(radio/radio.pro)
include(scrobble/scrobble.pro)
include( _files.qmake )

TEMPLATE = lib
TARGET = lastfm
QT = core network xml sql
VERSION = 0.2
INSTALLS = target
target.path = /lib
