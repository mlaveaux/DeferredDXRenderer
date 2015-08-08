#pragma once
#include <main/State.h>
#include <render/Camera.h>
#include <render/XMesh.h>

#include "object/Terrain.h"
#include "menu/EditMenu.h"

class EditorState : public IState
{
public:
	~EditorState();
	HRESULT Initialize();

	void handleEvent(Event& e);
	void render(float time);
	void tick();
private:
	Terrain*  pTerrain;
	EditMenu* pEditMenu;
	
	Camera*   m_pCamera;

	bool      bLButtonPressed;
	UVector2  mousePos;
	//Copies to 3d models.
	std::auto_ptr<XMesh> m_pSkydomeModel;
	std::auto_ptr<XMesh> m_pTankModel;
};