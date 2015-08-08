# - Try to find libGLM
# Once done this will define
#   GLM_FOUND - System has library.
#   GLM_INCLUDE_DIRS - include directories.
#   GLM_LIBRARIES - The libraries.

find_path(GLM_INCLUDE_DIR glm/glm.hpp
    PATHS ${CMAKE_SOURCE_DIR}/deps/glm/)

# set default Find****.cmake variables.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM DEFAULT_MESSAGE 
    GLM_INCLUDE_DIR)

set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIR})

mark_as_advanced(GLM_INCLUDE_DIR)

