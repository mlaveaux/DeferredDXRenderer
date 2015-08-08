#ifndef WATER_H
#define WATER_H

#include <render/DX11/DX11SceneManager.h>
#include <render/rendermodel.h>
#include <render/SceneNode.h>

/**
 * Class which represents water: oceans, lakes etc in the world map.
 */
class Water
{
public:
	//Create a specific water rendermodel.
	int initialize(DX11SceneManager* pSceneManager);

private:
	DX11SceneManager* m_pSceneManager;

	SceneNode*   m_sceneNode;
	RenderModel* m_waterModel;

	Texture m_normalTex;
	Texture m_depthTex;
	Texture m_reflectTex;
};

#endif // WATER_H