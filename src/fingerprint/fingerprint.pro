TEMPLATE = lib
TARGET = lastfm_fingerprint
LIBS += -L$$DESTDIR -llastfm
QT = core xml network sql
include( _files.qmake )
DEFINES += LASTFM_OHAI_QMAKE

win32 {
    #FIXME which -lfftw* is right?
    LIBS += -llibsamplerate -llibfftw3f-3 -lfftw3f
    DEFINES += __NO_THREAD_CHECK
    QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmt.lib
}
else{
    # versions break builds on Windows as it changes the link name! :P
    VERSION = 0.1

    mac:CONFIG( app_bundle ){
        LIBS += /opt/local/lib/libfftw3f.a /opt/local/lib/libsamplerate.a
        INCLUDEPATH += /opt/local/include
    }
    else{
        CONFIG += link_pkgconfig
        PKGCONFIG += fftw3f samplerate
    }
}

INSTALLS = target
target.path = /lib
