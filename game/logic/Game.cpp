#include "Game.h"

#include <fstream>

#include <main/GlobalFunctions.h>
#include <main/CVarManager.h>
#include <main/EventManager.h>

#include <render/CameraNode.h>
#include <render/SceneNode.h>

//Game States
#include "state/SPGameState.h"
#include "state/MenuState.h"
#include "state/EditorState.h"

Game::Game() : IEventListener("game")
{

}

Game::~Game()
{

}

HRESULT Game::Initialize(HINSTANCE hInstance, int cmdShow)
{
	//**********************************************
	//Initialize the engine for the game
	if(FAILED(engineCore.initialize(hInstance, cmdShow)))
		return E_FAIL;

	//get the main state manager
	m_pStateManager = engineCore.getStateManager();
	
	//*******************************
	//activate the standard state (MenuState)
	Event state("changestate", 1);
	state[0] = EventArg("menu");

	postEvent(state);

	dwTimeOld = GetTickCount();

	return S_OK;
}

void Game::handleEvent(Event& e)
{
	if(strcmp(e.getID(), "changestate") == 0)
	{
		if(strcmp(e[0].toChar(), "menu") == 0)
		{
			MenuState* pMenuState = dynamic_cast<MenuState*>(m_pStateManager->initState(new MenuState()));
			pMenuState->Initialize();

			m_pStateManager->setActiveState(pMenuState);
		}
		else if(strcmp(e[0].toChar(), "game") == 0)
		{			
			SPGameState* pGameState = dynamic_cast<SPGameState*>(m_pStateManager->initState(new SPGameState()));
			pGameState->Initialize();

			m_pStateManager->setActiveState(pGameState);
		}
		else if(strcmp(e[0].toChar(), "editor") == 0)
		{			
			EditorState* pEditorState = dynamic_cast<EditorState*>(m_pStateManager->initState(new EditorState()));
			pEditorState->Initialize();

			m_pStateManager->setActiveState(pEditorState);
		}
	}
}

void Game::interpolate()
{
	//***************************************************
	//Calculate the time difference
	//***************************************************
	DWORD dwTimeCurrent = GetTickCount();
    float time = (float)(dwTimeCurrent - dwTimeOld);	
	dwTimeOld = dwTimeCurrent;				

	engineCore.render(time);
}
