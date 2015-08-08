#ifndef SPGAMESTATE_H
#define SPGAMESTATE_H

#include <render/Camera.h>
#include <render/XMesh.h>
#include <gui/Font.h>
#include <main/client.h>
#include <main/State.h>

#include "logic/UnitHandler.h"
#include "object/Terrain.h"

#define TICK_DELAY 100

class SPGameState : public IState
{
public:
	SPGameState();
	~SPGameState();
	HRESULT Initialize();

	void handleEvent(Event& e);
	void render(float fTime);
	void tick(){};
private:
	Terrain*    pTerrain;
	Client      Client;
	UnitHandler UnitHandler;	

	Camera* m_pCamera;

	UVector2 CursorPos;
};

#endif