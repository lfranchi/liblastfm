find_package(Qt4 COMPONENTS QtSql REQUIRED)
find_package(Fftw3f REQUIRED)
find_package(Samplerate REQUIRED)

add_definitions(${QT_DEFINITIONS})

include_directories(
    ${QT_INCLUDES}
    ${FFTW3F_INCLUDE_DIRS}
    ${SAMPLERATE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
link_directories(${CURRENT_BINARY_DIR}/..)

set(liblastfm_fingerprint_LIBRARIES
    ${QT_QTCORE_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTSQL_LIBRARY}
    ${FFTW3F_LIBRARIES}
    ${SAMPLERATE_LIBRARIES}
    lastfm
)

set(liblastfm_fingerprint_SOURCES
    Collection.cpp
    Fingerprint.cpp
    Sha256.cpp
    fplib/Filter.cpp
    fplib/FingerprintExtractor.cpp
    fplib/OptFFT.cpp
)

set(liblastfm_fingerprint_HEADERS
    Fingerprint.h
    FingerprintableSource.h
)

if(WIN32)
    add_definitions("-DWIN32_LEAN_AND_MEAN")

    if(NOT MINGW)
        add_definitions("-D_ATL_DLL -D_CRT_SECURE_NO_WARNINGS")
    endif()
endif()

if(APPLE)
    #FIXME: enable this when deploying
    #set(CMAKE_OSX_ARCHITECTURES "i386;ppc")
    #set(CMAKE_OSX_DEPLOYMENT_TARGET 10.5)
    #set(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX${CMAKE_OSX_DEPLOYMENT_TARGET}.sdk")

    find_library(SYSTEMCONFIGURATION_LIBRARY SystemConfiguration)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    list(APPEND liblastfm_LIBRARIES
        ${COREFOUNDATION_LIBRARY}
        ${SYSTEMCONFIGURATION_LIBRARY}
    )
endif()

add_library(lastfm_fingerprint SHARED ${liblastfm_fingerprint_SOURCES})
target_link_libraries(lastfm_fingerprint ${liblastfm_fingerprint_LIBRARIES})
set_target_properties(lastfm_fingerprint PROPERTIES
    VERSION ${LASTFM_VERSION}
    SOVERSION ${LASTFM_SOVERSION}
    COMPILE_DEFINITIONS LASTFM_FINGERPRINT_LIB
)

install(TARGETS lastfm_fingerprint
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES ${liblastfm_fingerprint_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lastfm/)
