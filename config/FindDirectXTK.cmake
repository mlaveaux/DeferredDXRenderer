# - Try to find the required DirectXToolkit include and library directories.
# Once done this will define:
#   DIRECTXTK_FOUND - System has library.
#   DIRECTXTK_INCLUDE_DIRS - include directories.
#   DIRECTXTK_LIBRARIES - The libraries.

find_path(DIRECTXTK_INCLUDE_DIR SimpleMath.h
    PATHS ${CMAKE_SOURCE_DIR}/deps/DirectXTK/Inc/)

find_library(DIRECTXTK_LIBRARY NAMES DirectXTK.lib
    PATHS ${CMAKE_SOURCE_DIR}/deps/DirectXTK/Bin/Desktop_2013/Win32/Debug/)
		
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DirectXTK DEFAULT_MESSAGE 
    DIRECTXTK_INCLUDE_DIR DIRECTXTK_LIBRARY)

set(DIRECTXTK_INCLUDE_DIRS ${DIRECTXTK_INCLUDE_DIR})
set(DIRECTXTK_LIBRARIES ${DIRECTXTK_LIBRARY})

mark_as_advanced(DIRECTXTK_INCLUDE_DIR DIRECTXTK_LIBRARY)

