cmake_minimum_required(VERSION 2.8.6)
project(liblastfm)

# installation dirs
include(GNUInstallDirs)

# setup qt stuff
find_package(Qt4 COMPONENTS QtCore QtNetwork QtXml REQUIRED)
include_directories(${QT_INCLUDES} ${CMAKE_BINARY_DIR})
set(liblastfm_LIBRARIES
    ${QT_QTCORE_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTXML_LIBRARY}
)
set(CMAKE_AUTOMOC TRUE)

set(liblastfm_SOURCES
        src/ws.cpp
        src/NetworkConnectionMonitor.cpp
        src/NetworkAccessManager.cpp
        src/InternetConnectionMonitor.cpp
        src/Xspf.cpp
        src/User.cpp
        src/Track.cpp
        src/Tasteometer.cpp
        src/Tag.cpp
        src/Playlist.cpp
        src/Mbid.cpp
        src/FingerprintId.cpp
        src/Artist.cpp
        src/Album.cpp
        src/ScrobbleCache.cpp
        src/ScrobblePoint.cpp
        src/Audioscrobbler.cpp
        src/RadioTuner.cpp
        src/RadioStation.cpp
        src/XmlQuery.cpp
        src/UrlBuilder.cpp
        src/misc.cpp
        src/Chart.cpp
        src/Auth.cpp
        src/Library.cpp
)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-fno-operator-names -fvisibility-inlines-hidden -fvisibility=hidden")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-undefined")

if(WIN32)
    add_definitions("-DWIN32_LEAN_AND_MEAN")

    if(NOT MINGW)
        add_definitions("-D_ATL_DLL -D_CRT_SECURE_NO_WARNINGS")

        list(APPEND liblastfm_SOURCES
            src/win/WNetworkConnectionMonitor_win.cpp
            src/win/WmiSink.cpp
            src/win/Pac.cpp
            src/win/NdisEvents.cpp
        )
        list(APPEND liblastfm_LIBRARIES
            winhttp
            wbemuuid
        )
    endif()
endif()

if(APPLE)
    #FIXME
    #     QMAKE_PKGINFO_TYPEINFO = last
    #     QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4

    list(APPEND liblastfm_SOURCES
        src/mac/MNetworkConnectionMonitor_mac.cpp
    )
    find_library(SYSTEMCONFIGURATION_LIBRARY SystemConfiguration)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation)

    list(APPEND liblastfm_LIBRARIES
        ${COREFOUNDATION_LIBRARY}
        ${SYSTEMCONFIGURATION_LIBRARY}
    )
endif()

if(UNIX AND NOT APPLE)
    list(APPEND liblastfm_SOURCES
        src/linux/LNetworkConnectionMonitor_linux.cpp
        src/linux/LNetworkConnectionMonitor.h
    )

    find_package(Qt4 COMPONENTS QtDbus REQUIRED)
    list(APPEND liblastfm_LIBRARIES ${QT_QTDBUS_LIBRARY})
endif()

add_library(lastfm SHARED ${liblastfm_SOURCES})
target_link_libraries(lastfm ${liblastfm_LIBRARIES})
set_target_properties(lastfm PROPERTIES
    VERSION 1.0.0
    SOVERSION 1.0
    COMPILE_DEFINITIONS LASTFM_LIB
)

install(TARGETS lastfm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

file(GLOB liblastfm_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
install(FILES ${liblastfm_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/liblastfm/)
