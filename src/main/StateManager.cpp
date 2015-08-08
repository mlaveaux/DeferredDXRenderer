#include "StateManager.h"

StateManager::StateManager() : IEventListener("statemanager")
{
	m_pCurrentState = &m_idleState;
}

StateManager::~StateManager()
{

}

void StateManager::initialize(DX11SceneManager* pSceneManager, AudioSystem* pAudioSystem, GUInterface* pGUInterface)
{
	m_pSceneManager = pSceneManager;
	m_pAudioSystem  = pAudioSystem;
	m_pGUInterface  = pGUInterface;
}

void StateManager::render(float fTime)
{
	m_pCurrentState->render(fTime);
}

void StateManager::tick()
{
	m_pCurrentState->tick();
}

IState* StateManager::initState(IState* pState)
{
	pState->init(m_pSceneManager, m_pAudioSystem, m_pGUInterface);
	
	return pState;
}

void StateManager::setActiveState(IState* pState)
{
	if((long)m_pCurrentState != (long)&m_idleState)
		SAFE_DELETE(m_pCurrentState);
	
	m_pCurrentState = pState;
	
	if(pState == NULL)
		m_pCurrentState = &m_idleState;
}

void StateManager::handleEvent(Event& e)
{
	m_pCurrentState->handleEvent(e);
}