#ifndef UNIT_H
#define UNIT_H

#include <string> 
#include "render/DX11/DX11SceneManager.h"
#include "render/SceneNode.h"
#include "render/XMesh.h"

struct Waypoint
{
	glm::vec2 Pos;
	float Direction;
};

//BaseClass for every unit
class BaseUnit
{
public:
	BaseUnit(DX10SceneManager* pSceneManager);	
	XMesh* getMesh() { return NULL; }
protected:
	//XMesh m_mesh;
	float m_maxHealth;
private:
	DX10SceneManager* m_pSceneManager;
};

class UnitController
{
public:
	UnitController(DX10SceneManager* pSceneManager, BaseUnit* pUnit);

	void   setPosition(glm::vec2 pos);
private:
	float     m_health;

	SceneNode* m_pSceneNode;
	BaseUnit*  m_pBaseUnit;
};

//Example unit
class Tank : public BaseUnit
{
public:
	Tank(DX10SceneManager* pSceneManager);
	HRESULT initialize();
private:	
};

#endif