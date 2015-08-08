#include "SPGameState.h"

#include <main/ConVar.h>
#include <main/EventManager.h>
#include <main/client.h>

ConVar team("cl_team", CValue(0), CVAR_FNONE, "");

SPGameState::SPGameState()
{

}

SPGameState::~SPGameState()
{
	SAFE_DELETE(pTerrain);
}

HRESULT SPGameState::Initialize()
{
	ConVar r_windowSize("r_windowsize");

	//Create a camera
	m_pCamera = new Camera(r_windowSize.toVector().x, r_windowSize.toVector().y, 65.0f);
	m_pCamera->setLookAt(20.0f, 0.0f, 20.0f);
	//m_pCamera->setPosition(30.0f, 10.0f, 25.0f);

	SceneManager()->setCamera(m_pCamera);

	pTerrain = new Terrain(SceneManager());

	if(FAILED(pTerrain->create(UVector2(64, 64))))
		return E_FAIL;

	//if(FAILED(UnitHandler.Initialize(SceneManager())))
	//	return E_FAIL;

	//pMaxPlayers = m_pCVarSystem->GetRefByName("sv_maxplayers");

	//m_pTeam       = CVarManager::Instance().Find("cl_team");
	//pSpecMode   = m_pCVarSystem->GetVar("cl_specmode");

	//pPlayer = new Entity[pMaxPlayers->toInt()];
	
	//UnitHandler.Spawn("basictank", D3DXVECTOR2(5.0f, 5.0f));

	//if(FAILED(LoadSound(&m_sound, "..\\data\\sounds\\Background_Music.wav")))
	//	::PostQuitMessage(0);
	
	//pAudioSystem->PlaySound3D(&m_sound, 5.0f, 1.0f, -5.0f);
	
	//UInterface()->GetMainElement()->Set(NULL);
	
	//fTimeTick = 0.0f;

	return S_OK;
}

void SPGameState::handleEvent(Event& e)
{
	/*switch(uMessage)
	{
	case WM_KEYDOWN:
		{
		if(wParam == VK_ESCAPE)
			EventManager::Instance().postEvent(EVT_MENUSTATE, 0, 0);
		break;
		}
	case WM_MOUSEMOVE:
		{
			CursorPos.x = (unsigned int)LOWORD(lParam);
			CursorPos.y = (unsigned int)HIWORD(lParam);
			break;
		}
	case WM_LBUTTONDOWN:
		{
			D3DXVECTOR3 RayOrigin;
			D3DXVECTOR3 RayDirection;
			D3DXVECTOR3 HitPos;

			SceneManager()->mainCamera()->calculateRay(CursorPos, RayOrigin, RayDirection);

			UnitHandler.Select(RayOrigin, RayDirection);
		break;
		}
	case WM_RBUTTONDOWN:
		{
		break;
		}
	}*/
}

void SPGameState::render(float time)
{	

}