TEMPLATE = lib
TARGET = lastfm_fingerprint
LIBS += -L$$DESTDIR -llastfm
QT = core xml network sql
include( _files.qmake )
DEFINES += LASTFM_OHAI_QMAKE

INSTALLS = target
target.path = /lib

mac:CONFIG( app_bundle ) {
    LIBS += libfftw3f.a libsamplerate.a -L/opt/local/include
    INCLUDEPATH += /opt/local/include
}else{
    CONFIG += link_pkgconfig
    PKGCONFIG += samplerate
    win32 {
        CONFIG += link_pkgconfig
        DEFINES += __NO_THREAD_CHECK
        QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmt.lib
	}
    PKGCONFIG += fftw3f

}
