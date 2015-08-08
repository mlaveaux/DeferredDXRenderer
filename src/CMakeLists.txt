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
project(engine)

prepend_dir(${CMAKE_SOURCE_DIR} HEADERS
	src/audio/AudioSystem.h
	src/game/Component.h
	src/game/Entity.h
	src/game/EntityManager.h
	src/gui/controls/Button.h
	src/gui/controls/RadioButton.h
	src/gui/controls/Slider.h
	src/gui/controls/Textbox.h
	src/gui/DebugWND.h
	src/gui/Font.h
	src/gui/GUIElement.h
	src/gui/GUInterface.h
	src/gui/MainElement.h
	src/gui/UserWND.h
	src/main/Client.h
	src/main/ConVar.h
	src/main/CVarManager.h
	src/main/EngineCore.h
	src/main/Event.h
	src/main/EventFn.h
	src/main/EventManager.h
	src/main/globalFunctions.h
	src/main/globals.h
	src/main/KeybindManager.h
	src/main/MessageFn.h
	src/main/RefObject.h
	src/main/State.h
	src/main/StateManager.h
	src/render/Camera.h
	src/render/CameraNode.h
	src/render/DebugElement.h
	src/render/DX11/DX11Context.h
	src/render/DX11/DX11ParticleSystem.h
	src/render/DX11/DX11RenderSystem.h
	src/render/DX11/DX11SceneManager.h
	src/render/DX11/DX11Shader.h
	src/render/DX11/DX11ShaderSystem.h
	src/render/DX11/DX11Surface.h
	src/render/DX11/DX11Texture.h
	src/render/DX11/DX11Global.h
	src/render/IRenderObject.h
	src/render/RenderModel.h
	src/render/RenderSystem.h
	src/render/SceneNode.h
	src/render/VertexTypes.h
	src/render/XMesh.h
	)
	
prepend_dir(${CMAKE_SOURCE_DIR} SOURCES
	src/audio/AudioSystem.cpp
	src/gui/controls/Textbox.cpp
	src/gui/DebugWND.cpp
	src/gui/Font.cpp
	src/gui/GUIElement.cpp
	src/gui/GUInterface.cpp
	src/gui/MainElement.cpp
	src/gui/UserWND.cpp
	src/main/Client.cpp
	src/main/ConVar.cpp
	src/main/CVarManager.cpp
	src/main/EngineCore.cpp
	src/main/EventManager.cpp
	src/main/globalFunctions.cpp
	src/main/KeybindManager.cpp
	src/main/StateManager.cpp
	src/render/Camera.cpp
	src/render/DebugElement.cpp
	src/render/DX11/DX11Context.cpp
	src/render/DX11/DX11ParticleSystem.cpp
	src/render/DX11/DX11RenderSystem.cpp
	src/render/DX11/DX11SceneManager.cpp
	src/render/DX11/DX11Shader.cpp
	src/render/DX11/DX11ShaderSystem.cpp
	src/render/DX11/DX11Surface.cpp
	src/render/DX11/DX11Texture.cpp
	src/render/DX11/DX11Global.cpp
	src/render/XMesh.cpp
	)
	
find_package(DirectX11 REQUIRED)
find_package(DirectXTK REQUIRED)
find_package(DirectXEffects REQUIRED)
find_package(XAudio2 REQUIRED)
find_package(glm REQUIRED)

include_directories(
	${CMAKE_SOURCE_DIR}/src/
	${D3D11_INCLUDE_DIRS}
	${XAUDIO2_INCLUDE_DIRS}
	${DIRECTXTK_INCLUDE_DIRS}
	${DX11EFFECTS_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS})
	
add_library(engine STATIC 
	${HEADERS} 
	${SOURCES})

target_link_libraries(engine
	${D3D11_LIBRARIES}
	${DX11EFFECTS_LIBRARIES}
	${XAUDIO2_LIBRARIES}
	${DIRECTXTK_LIBRARIES})