# - Try to find the required DirectX11 include and library directories.
# Once done this will define:
#   D3DX11_FOUND - System has library.
#   D3DX11_INCLUDE_DIRS - include directories.
#   D3DX11_LIBRARIES - The libraries.

find_path(D3D11_INCLUDE_DIR d3d11_2.h
    PATHS /test/)

find_library(D3D11_LIBRARY NAMES d3d11.lib
        PATHS /test/)
		
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DirectX11 DEFAULT_MESSAGE 
    D3D11_INCLUDE_DIR D3D11_LIBRARY)

set(D3D11_INCLUDE_DIRS ${D3DX11_INCLUDE_DIR})
set(D3D11_LIBRARIES ${D3DX11_LIBRARY})

mark_as_advanced(D3D11_INCLUDE_DIR D3D11_LIBRARY)

