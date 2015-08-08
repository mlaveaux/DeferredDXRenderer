# - Try to find the required XAudio2 include and library directories.
# Once done this will define:
#   XAUDIO2_FOUND - System has library.
#   XAUDIO2_INCLUDE_DIRS - include directories.
#   XAUDIO2_LIBRARIES - The libraries.

find_path(XAUDIO2_INCLUDE_DIR xaudio2.h
    PATHS /test/)

find_library(XAUDIO2_LIBRARY NAMES xaudio2.lib
        PATHS /test/)
		
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XAudio2 DEFAULT_MESSAGE 
    XAUDIO2_INCLUDE_DIR XAUDIO2_LIBRARY)

set(XAUDIO2_INCLUDE_DIRS ${XAUDIO2_INCLUDE_DIR})
set(XAUDIO2_LIBRARIES ${XAUDIO2_LIBRARY})

mark_as_advanced(XAUDIO2_INCLUDE_DIR XAUDIO2_LIBRARY)

