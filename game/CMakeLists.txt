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
project(game)

prepend_dir(${CMAKE_SOURCE_DIR} HEADERS
	game/logic/Game.h
	game/logic/UnitHandler.h
	game/menu/EditMenu.h
	game/menu/MainMenu.h
	game/menu/QuitDialog.h
	game/object/Entity.h
	game/object/Tank.h
	game/object/Terrain.h
	game/object/TerrainChunk.h
	game/object/Unit.h
	game/object/Water.h
	game/state/EditorState.h
	game/state/MenuState.h
	game/state/MPGameState.h
	game/state/SPGameState.h)
	
prepend_dir(${CMAKE_SOURCE_DIR} SOURCES
	game/logic/Game.cpp
	game/logic/UnitHandler.cpp
	game/Main.cpp
	game/menu/EditMenu.cpp
	game/menu/MainMenu.cpp
	game/menu/QuitDialog.cpp
	game/object/Entity.cpp
	game/object/Tank.cpp
	game/object/Terrain.cpp
	game/object/TerrainChunk.cpp
	game/object/Unit.cpp
	game/object/Water.cpp
	game/state/EditorState.cpp
	game/state/MenuState.cpp
	game/state/SPGameState.cpp)
	
find_package(glm REQUIRED)
find_package(DirectXTK REQUIRED)
	
include_directories(
	${CMAKE_SOURCE_DIR}/game/
	${CMAKE_SOURCE_DIR}/src/
	${GLM_INCLUDE_DIRS}
	${DIRECTXTK_INCLUDE_DIRS})
	
add_executable(game 
	${HEADERS} 
	${SOURCES})

target_link_libraries(game 
	engine)