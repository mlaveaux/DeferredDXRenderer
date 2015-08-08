# - Try to find the required DirectX Effects include and library directories.
# Once done this will define:
#   DX11EFFECTS_FOUND - System has library.
#   DX11EFFECTS_INCLUDE_DIRS - include directories.
#   DX11EFFECTS_LIBRARIES - The libraries.

find_path(DX11EFFECTS_INCLUDE_DIR d3dx11effect.h
    PATHS ${CMAKE_SOURCE_DIR}/deps/DirectXEffects/Inc/)

find_library(DX11EFFECTS_LIBRARY NAMES Effects11.lib
    PATHS ${CMAKE_SOURCE_DIR}/deps/DirectXEffects/Bin/Desktop_2013/Win32/Debug/)
		
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DirectXEffects DEFAULT_MESSAGE 
    DX11EFFECTS_INCLUDE_DIR DX11EFFECTS_LIBRARY)

set(DX11EFFECTS_INCLUDE_DIRS ${DX11EFFECTS_INCLUDE_DIR})
set(DX11EFFECTS_LIBRARIES ${DX11EFFECTS_LIBRARY})

mark_as_advanced(DX11EFFECTS_INCLUDE_DIR DX11EFFECTS_LIBRARY)

