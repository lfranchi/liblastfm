TEMPLATE = lib
TARGET = lastfm
QT = core network xml
INSTALLS = target
include( _files.qmake )

win32{
    DEFINES += LASTFM_OHAI_QMAKE _ATL_DLL 
    LIBS += winhttp.lib wbemuuid.lib # ws configuration
}
else{
    # don't break the link name on Windows, otherwise you get eg. lastfm1.dll
    VERSION = 0.3
    mac{
        LIBS += -framework SystemConfiguration # ws configuration
        #TODO we should only use these with the carbon version of Qt!
        LIBS += -framework Carbon -framework CoreFoundation # various
    }
}

target.path = /lib