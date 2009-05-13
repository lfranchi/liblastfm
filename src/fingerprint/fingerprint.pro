TEMPLATE = lib
TARGET = lastfm_fingerprint
LIBS += -L$$DESTDIR -llastfm
QT = core xml network sql
include( _files.qmake )
DEFINES += LASTFM_OHAI_QMAKE

win32 {
    LIBS += -llibsamplerate -llibfftw3f-3 -lmad
    DEFINES += __NO_THREAD_CHECK
    QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmt.lib
    LIBS += -lmad -lfftw3f
}
else{
    # versions break builds on Windows as it changes the link name! :P
    VERSION = 0.1
    
    mac:CONFIG( app_bundle ){
	    LIBS += /opt/local/lib/libmad.a /opt/local/lib/libfftw3f.a /opt/local/lib/libsamplerate.a
	    INCLUDEPATH += /opt/local/include
    }
    else{
        CONFIG += link_pkgconfig
        PKGCONFIG += mad fftw3f samplerate
        LIBS += -lvorbisfile
    }
}

INSTALLS = target
target.path = /lib
