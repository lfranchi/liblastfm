TEMPLATE = lib
TARGET = lastfm_fingerprint
LIBS += -L$$DESTDIR -llastfm
QT = core xml network sql
include( _files.qmake )
DEFINES += LASTFM_OHAI_QMAKE

INSTALLS = target
target.path = /lib

win32 {
	CONFIG += link_pkgconfig
	PKGCONFIG += fftw3 libsamplerate
    DEFINES += __NO_THREAD_CHECK
    QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmt.lib
}
else:mac:CONFIG( app_bundle ){
    LIBS += libfftw3f.a libsamplerate.a -L/opt/local/include
    INCLUDEPATH += /opt/local/include
}
else{
    CONFIG += link_pkgconfig
    PKGCONFIG += fftw3f samplerate
}