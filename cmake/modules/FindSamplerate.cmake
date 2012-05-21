# - Try to find the Samplerate include dir and library
# Variables defined by this module:
#
#  SAMPLERATE_INCLUDE_DIRS - where to find samplerate.h
#  SAMPLERATE_LIBRARIES - List of libraries when using Samplerate
#  SAMPLERATE_FOUND - true if Samplerate found

find_path(SAMPLERATE_INCLUDE_DIR NAMES samplerate.h)

find_library(SAMPLERATE_LIBRARY
    NAMES samplerate libsamplerate
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SAMPLERATE DEFAULT_MSG
    SAMPLERATE_LIBRARY SAMPLERATE_INCLUDE_DIR
)

if(SAMPLERATE_FOUND)
    set(SAMPLERATE_INCLUDE_DIRS ${SAMPLERATE_INCLUDE_DIR})
    set(SAMPLERATE_LIBRARIES ${SAMPLERATE_LIBRARY})
endif()

mark_as_advanced(SAMPLERATE_INCLUDE_DIR SAMPLERATE_LIBRARY)
