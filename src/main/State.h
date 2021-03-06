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

#ifndef ISTATE_H
#define ISTATE_H

#include "Globals.h"
#include "EventManager.h"

#include "render/DX11/DX11RenderSystem.h"
#include "audio/AudioSystem.h"
#include "gui/GUInterface.h"

#include <windows.h>

//Every state in the engine is derived from this class.
class IState
{
public:	
	IState(){} 
	virtual ~IState(){};

	virtual void handleEvent(Event& e) = 0;
	virtual void render(float fTime) = 0;
	virtual void tick() = 0;

	void init(DX11SceneManager* pSceneManager, AudioSystem* pAudioSystem, GUInterface* pGUInterface)
	{
		m_pSceneManager = pSceneManager;
		this->pAudioSystem  = pAudioSystem;
		this->pGUInterface  = pGUInterface;
	}
protected:		
	DX11SceneManager* SceneManager() { return m_pSceneManager; }
	
	AudioSystem*      pAudioSystem;
	GUInterface*      pGUInterface;
private:
	//These are always available.
	DX11SceneManager* m_pSceneManager;
};

#endif // ISTATE_H