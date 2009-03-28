TEMPLATE = lib
QT = core network xml
include( _files.qmake )
DEFINES += _TYPES_DLLEXPORT
headers.files = Track.h Mbid.h Artist.h Album.h FingerprintId.h Playlist.h Tag.h User.h Xspf.h
