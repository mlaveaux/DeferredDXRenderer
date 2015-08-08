#ifndef MENUSTATE_H
#define MENUSTATE_H

#include <game/EntityManager.h>

#include <render/XMesh.h>
#include <render/SceneNode.h>
#include <main/State.h>
#include <gui/Font.h>

#include "menu/MainMenu.h"

//The menu state before the game starts
class MenuState : public IState
{
public:
	MenuState();
	~MenuState();
	int Initialize();
	void handleEvent(Event& e){}
	void render(float time);
	void tick() {}
private:
	MainMenu* m_pMainMenu;
	XMesh*    m_pPlanetModel;

	//Pointer to the SceneNode of m_pPlanetModel
	SceneNode*   m_pPlanetNode;
	CameraNode*  m_pCamera;

	//Backgroundsound
	UINT m_backgroundMusic;
	UINT m_backgroundEmitter;
		
	float m_equatorangle;
};

#endif