#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include "object/Unit.h"

#include <windows.h>
#include <vector>
#include <glm/glm.hpp>

class DX10SceneManager;
class BaseUnit;
class UnitController;

class UnitHandler
{
public:
	HRESULT Initialize(DX10SceneManager* pSceneManager);

	//Unit Functions
	void Spawn(const char* strUnit, glm::vec2 Pos);

	void Select(glm::vec3 Point, glm::vec3 Direction);
	void Render(float fTime);
	void MouseClick();
	void Tick();
	void Release(){};
private:
	DX10SceneManager* m_pSceneManager;

	std::vector<BaseUnit*>      m_baseUnitArray;
	std::vector<UnitController> m_unitArray;
};

#endif