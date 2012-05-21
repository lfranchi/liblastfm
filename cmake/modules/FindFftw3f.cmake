# - Try to find the fftw3f include dir and library
# Variables defined by this module:
#
#  FFTW3F_INCLUDE_DIRS - where to find fftw3.h
#  FFTW3F_LIBRARIES - list of libraries when using fftw3f
#  FFTW3F_FOUND - true if Fftw3f found

find_path(FFTW3F_INCLUDE_DIR NAMES fftw3.h)

find_library(FFTW3F_LIBRARY
    NAMES fftw3f libfftw3f
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Fftw3f DEFAULT_MSG
    FFTW3F_LIBRARY FFTW3F_INCLUDE_DIR
)

if(FFTW3F_FOUND)
    set(FFTW3F_INCLUDE_DIRS ${FFTW3F_INCLUDE_DIR})
    set(FFTW3F_LIBRARIES ${FFTW3F_LIBRARY})
endif()

mark_as_advanced(FFTW3F_INCLUDE_DIR FFTW3F_LIBRARY)
