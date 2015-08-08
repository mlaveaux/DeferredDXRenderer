#include "UnitHandler.h"

#include <render/DX10/DX10RenderSystem.h>

HRESULT UnitHandler::Initialize(DX10SceneManager* pSceneManager)
{
	m_pSceneManager = pSceneManager;

	m_baseUnitArray.reserve(256);

	Tank* pBasicTank = new Tank(m_pSceneManager);
	if(pBasicTank->initialize() != S_OK)
		return E_FAIL;

	m_baseUnitArray.push_back(pBasicTank);

	return S_OK;
}

void UnitHandler::Spawn(const char* strUnit, glm::vec2 Pos)
{
	UnitController unit(m_pSceneManager, m_baseUnitArray[0]);
	unit.setPosition(Pos);

	m_unitArray.push_back(unit);
}

void UnitHandler::Select(glm::vec3 RayOrigin, glm::vec3 RayDirection)
{
	/*UINT i = 0;

	D3DXMATRIX World;
	D3DXVECTOR3 UnitPos;
	D3DXVECTOR2 ClickPos;

	float fDelta = 0.0f;
	bool  bFound = false;

	//Select one unit in by distance to a ray
	for(i = 0;i < uMaxUnits;i++)
	{
		if(pUnitArray[i] != NULL)
		{			
			if(!bFound)
			{
				World = pUnitArray[i]->GetWorldMatrix();

				UnitPos = D3DXVECTOR3(World._41, World._42, World._43);

				fDelta = RayOrigin.y - UnitPos.y;
				fDelta /= RayDirection.y;

				if(fDelta < 0.0f)
					fDelta *= -1.0f;

				ClickPos.x = fDelta * RayDirection.x + RayOrigin.x;
				ClickPos.y = fDelta * RayDirection.z + RayOrigin.z;

				if(::D3DXVec2Length(&D3DXVECTOR2(UnitPos.x - ClickPos.x, UnitPos.z - ClickPos.y)) < 1.0f)
				{
					pUnitArray[i]->SetSelectState(true);
					bFound = true;
				}
			}
		}
	}*/
}

void UnitHandler::Render(float fTime)
{
	/*for(UINT i = 0;i < 1;i++)
	{
		if(pUnitArray[i] != NULL)
			pUnitArray[i]->Render(fTime);
	}*/
}