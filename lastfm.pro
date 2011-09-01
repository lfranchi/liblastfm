TEMPLATE = lib
QT = core network xml

INSTALLS = target
target.path = /lib

CONFIG += dll

win32{
    DEFINES += LASTFM_LIB _ATL_DLL 
    LIBS += winhttp.lib wbemuuid.lib # ws configuration
}
mac{
    LIBS += -framework SystemConfiguration # ws configuration
    #TODO we should only use these with the carbon version of Qt!
    LIBS += -framework Carbon -framework CoreFoundation # various
}

linux*{
    QT += dbus
}

SOURCES += \
	src/ws/ws.cpp \
	src/ws/NetworkConnectionMonitor.cpp \
	src/ws/NetworkAccessManager.cpp \
	src/ws/InternetConnectionMonitor.cpp \
	src/types/Xspf.cpp \
	src/types/User.cpp \
	src/types/Track.cpp \
	src/types/Tasteometer.cpp \
	src/types/Tag.cpp \
	src/types/Playlist.cpp \
	src/types/Mbid.cpp \
	src/types/FingerprintId.cpp \
	src/types/Artist.cpp \
	src/types/Album.cpp \
	src/scrobble/ScrobbleCache.cpp \
	src/scrobble/Audioscrobbler.cpp \
	src/radio/RadioTuner.cpp \
	src/radio/RadioStation.cpp \
	src/core/XmlQuery.cpp \
	src/core/UrlBuilder.cpp \
	src/core/misc.cpp
	
HEADERS += \
	src/ws/ws.h \
	src/ws/NetworkConnectionMonitor.h \
	src/ws/NetworkAccessManager.h \ 
	src/ws/InternetConnectionMonitor.h \
	src/types/Xspf.h \
	src/types/User.h \
	src/types/Track.h \
	src/types/Tasteometer.h \
	src/types/Tag.h \
	src/types/Playlist.h \
	src/types/Mbid.h \
	src/types/FingerprintId.h \
	src/types/Artist.h \
	src/types/Album.h \
	src/types/AbstractType.h \
	src/scrobble/ScrobblePoint.h \
	src/scrobble/ScrobbleCache.h \
	src/scrobble/Audioscrobbler.h \
	src/radio/RadioTuner.h \
	src/radio/RadioStation.h \
	src/global.h \
	src/core/XmlQuery.h \
	src/core/UrlBuilder.h \
	src/core/misc.h
	
win32:SOURCES += src/ws/win/WNetworkConnectionMonitor_win.cpp \
	src/ws/win/WmiSink.cpp \
	src/ws/win/Pac.cpp \
	src/ws/win/NdisEvents.cpp
	
win32:HEADERS += src/ws/win/WNetworkConnectionMonitor.h \
	src/ws/win/WmiSink.h \
	src/ws/win/Pac.h \
	src/ws/win/NdisEvents.h \
	src/ws/win/IeSettings.h \
	src/ws/win/ComSetup.h

mac:SOURCES += 	src/ws/mac/MNetworkConnectionMonitor_mac.cpp

mac:HEADERS += src/ws/mac/ProxyDict.h \
               src/ws/mac/MNetworkConnectionMonitor.h

!win32:VERSION = 0.4.0
