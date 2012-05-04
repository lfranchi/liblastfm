cmake_minimum_required(VERSION 2.8.6)
project(liblastfm)

# general settings
set(LASTFM_VERSION 1.0.0)
set(LASTFM_SOVERSION 1.0)

# options
option(BUILD_FINGERPRINT "Build the lastfm-fingerprint library" OFF)
option(BUILD_DEMOS "Build the lastfm example programs" OFF)

# installation dirs
include(GNUInstallDirs)

#cmake module path
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake/Modules")

# setup qt stuff
find_package(Qt4 COMPONENTS QtCore QtNetwork QtXml REQUIRED)
set(CMAKE_AUTOMOC TRUE)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-fno-operator-names -fvisibility-inlines-hidden -fvisibility=hidden")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-undefined")

# main library
add_subdirectory(src)

# libfingerprint
if(BUILD_FINGERPRINT)
    add_subdirectory(src/fingerprint)
endif()

# demos
if(BUILD_DEMOS)
    add_subdirectory(demos)
endif()
