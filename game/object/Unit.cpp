#include "Unit.h"

UnitController::UnitController(DX10SceneManager* pSceneManager, BaseUnit* pUnit)
{
	m_pBaseUnit = pUnit;

	m_pSceneNode = pSceneManager->createSceneNode("tank", pSceneManager->getMainNode());
	//m_pSceneNode->attachModel(pUnit->getMesh());

	m_health = 10;

	//m_health = m_pBaseUnit->getMaxHealth();
}

void UnitController::setPosition(glm::vec2 pos)
{
	m_pSceneNode->identity();
	m_pSceneNode->translate(pos.x, 0.0f, pos.y);
}

BaseUnit::BaseUnit(DX10SceneManager* pSceneManager)
{
	m_pSceneManager = pSceneManager;
}

Tank::Tank(DX10SceneManager* pSceneManager) : BaseUnit(pSceneManager)
{

}

HRESULT Tank::initialize()
{
	//Initialize the basic things of a tank.
	//if(FAILED(m_mesh.Load("..\\data\\meshes\\mesh.x")))
	{
	//	return E_FAIL;
	}

	m_maxHealth = 200;

	return S_OK;
}
