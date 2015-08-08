# Copyright 2012-2015 Maurice Laveaux

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.0)
	
project(DeferredDXRenderer)

# define a function to prepend a given DIRECTORY to every files passed and set them in VARIABLE_NAME
function(prepend_dir DIRECTORY VARIABLE_NAME)
    foreach(SOURCE_FILE ${ARGN})
        set(MODIFIED ${MODIFIED} ${DIRECTORY}/${SOURCE_FILE})
    endforeach()
    set(${VARIABLE_NAME} ${MODIFIED} PARENT_SCOPE)
endfunction(prepend_dir)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/config)

add_subdirectory(game/)
add_subdirectory(src/)