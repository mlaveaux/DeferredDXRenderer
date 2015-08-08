/*******************************************
Engine which handles input and displays
ouput.
*******************************************/
#pragma once
#include <windows.h>
#include <string.h>

#ifdef _DEBUG
#pragma comment(lib, "Engine_debug.lib")
#else
#pragma comment(lib, "Engine.lib")
#endif

#include "main/EngineCore.h"
#include "main/StateManager.h"

//Main game class, where the game starts.
class Game : public IEventListener
{
public:
	Game();
	~Game();
	HRESULT Initialize(HINSTANCE hInstance, int cmdShow);
	void handleEvent(Event& e);
	void interpolate();
private:
	//Main ClockWorks(c) engine core.
	EngineCore engineCore;
	//State Manager for the engine.	
	StateManager* m_pStateManager;
		
	//Time variables
	DWORD dwTimeOld;
};