/* Copyright 2012-2015 Maurice Laveaux
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef EngineCore_H
#define EngineCore_H

#include "EventManager.h"
#include "StateManager.h"
#include "KeybindManager.h"

#include "render/DX11/DX11RenderSystem.h"
#include "render/DebugElement.h"
#include "audio/AudioSystem.h"

#include "gui/GUInterface.h"
#include "gui/UserWND.h"
#include "gui/DebugWND.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <memory>

/**
 * Engine Core:
 * maintains all major classes of the engine.
 * main output devices: RenderSystem, GUInterface and AudioManager.
 */
class EngineCore : public IEventListener
{
public:
	EngineCore();
	~EngineCore();
	//Initializes the main classes and windows.
	HRESULT initialize(HINSTANCE hInstance, int cmdShow);

	StateManager*     getStateManager();
	DX11RenderSystem* getRenderSystem();
	GUInterface*      getGUInterface();

	void handleEvent(Event& e);

	void render(float fTime);
	void tick();
private:
	Event   parse(std::string strCommand);
	HRESULT loadConfig(const char* strFilename);
	//Counts 100ms till a tick passes.
	float m_timeLeft;

	//Core windows required for the engine
	UserWND  m_userWnd;
	DebugWND m_debugWnd;

	//Base classes of the engine
	std::unique_ptr<DX11RenderSystem> m_pRenderSystem;
	GUInterface       m_gUInterface;
	AudioSystem       m_audioSystem;
	StateManager      m_stateManager;
	KeybindManager    m_keybindManager;
	
	DWORD	          m_dwTimeOld;
};

#endif