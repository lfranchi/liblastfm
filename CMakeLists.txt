cmake_minimum_required(VERSION 2.8.6)
project(liblastfm)

# installation dirs
include(GNUInstallDirs)

# setup qt stuff
find_package(Qt4 COMPONENTS QtCore QtNetwork QtXml REQUIRED)
set(CMAKE_AUTOMOC TRUE)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-fno-operator-names -fvisibility-inlines-hidden -fvisibility=hidden")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-undefined")

# main library
add_subdirectory(src)
