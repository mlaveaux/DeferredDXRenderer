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

#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include "State.h"
#include "EventManager.h"

class IdleState : public IState
{
public:
	void handleEvent(Event& e){}
	void render(float fTime){}
	void tick(){}
private:
};

/**
 * Main class that manages and creates game states.
 */
class StateManager : public IEventListener
{
public:
	//Constructor
	StateManager();
	~StateManager();

	void initialize(DX11SceneManager* pSceneManager, AudioSystem* pAudioSystem, GUInterface* pGUInterface);

	//renders the current active state
	void render(float fTime);

	//Gametick
	void tick();

	//creates a state.
	IState* initState(IState* pState);

	//change the current state
	void setActiveState(IState* pState);

	//override handleEvent().
	void handleEvent(Event& e);
private:
	IdleState  m_idleState;
	IState*    m_pCurrentState;

    DX11SceneManager* m_pSceneManager;
	AudioSystem*      m_pAudioSystem;
	GUInterface*      m_pGUInterface;
};

#endif // STATEMANAGER_H