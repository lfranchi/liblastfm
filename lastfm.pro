TEMPLATE = lib
CONFIG += dll
QT = core network xml

INSTALLS = target headers
target.path = /usr/local/lib
headers.path = /usr/local/include/lastfm
headers.files = src/*.h

INCLUDEPATH += src

win32{
    DEFINES += LASTFM_LIB _ATL_DLL 
    LIBS += winhttp.lib wbemuuid.lib # ws configuration
}
mac{
    LIBS += -framework SystemConfiguration # ws configuration
    #TODO we should only use these with the carbon version of Qt!
    LIBS += -framework CoreFoundation # various
}

linux*{
    QT += dbus
}

SOURCES += \
        src/ws.cpp \
        src/NetworkConnectionMonitor.cpp \
        src/NetworkAccessManager.cpp \
        src/InternetConnectionMonitor.cpp \
        src/Xspf.cpp \
        src/User.cpp \
        src/Track.cpp \
        src/Tasteometer.cpp \
        src/Tag.cpp \
        src/Playlist.cpp \
        src/Mbid.cpp \
        src/FingerprintId.cpp \
        src/Artist.cpp \
        src/Album.cpp \
        src/ScrobbleCache.cpp \
        src/Audioscrobbler.cpp \
        src/RadioTuner.cpp \
        src/RadioStation.cpp \
        src/XmlQuery.cpp \
        src/UrlBuilder.cpp \
        src/misc.cpp
	
HEADERS += \
        src/ws.h \
        src/NetworkConnectionMonitor.h \
        src/NetworkAccessManager.h \
        src/InternetConnectionMonitor.h \
        src/Xspf.h \
        src/User.h \
        src/Track.h \
        src/Tasteometer.h \
        src/Tag.h \
        src/Playlist.h \
        src/Mbid.h \
        src/FingerprintId.h \
        src/Artist.h \
        src/Album.h \
        src/AbstractType.h \
        src/ScrobblePoint.h \
        src/ScrobbleCache.h \
        src/Audioscrobbler.h \
        src/RadioTuner.h \
        src/RadioStation.h \
	src/global.h \
        src/XmlQuery.h \
        src/UrlBuilder.h \
        src/misc.h
	
win32:SOURCES += src/win/WNetworkConnectionMonitor_win.cpp \
        src/win/WmiSink.cpp \
        src/win/Pac.cpp \
        src/win/NdisEvents.cpp
	
win32:HEADERS += src/win/WNetworkConnectionMonitor.h \
        src/win/WmiSink.h \
        src/win/Pac.h \
        src/win/NdisEvents.h \
        src/win/IeSettings.h \
        src/win/ComSetup.h

mac:SOURCES += 	src/mac/MNetworkConnectionMonitor_mac.cpp

mac:HEADERS += src/mac/ProxyDict.h \
               src/mac/MNetworkConnectionMonitor.h

!win32:VERSION = 0.4.0
