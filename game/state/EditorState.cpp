#include "EditorState.h"

#include <main/ConVar.h>
#include <main/EventManager.h>

ConVar editType(  "terrain_edittype", CValue(0), CVAR_FNONE, "");
ConVar editRadius("terrain_radius"  , CValue(10.0f), CVAR_FNONE, "");
ConVar editStrenght("terrain_strength", CValue(2.0f), CVAR_FNONE, "");

ConVar editorType("editor_type"    , CValue(0), CVAR_FNONE, "");

EditorState::~EditorState()
{
	SAFE_DELETE(pEditMenu);
	SAFE_DELETE(pTerrain);
	SAFE_DELETE(m_pCamera);
}

HRESULT EditorState::Initialize()
{
	ConVar r_windowSize("r_windowsize");

	UVector2 terrainSize(128, 128);

	//Create a camera
	m_pCamera = new Camera(r_windowSize.toVector().x, r_windowSize.toVector().y, 65.0f);
	m_pCamera->setLookAt(20.0f, 0.0f, 20.0f);
	m_pCamera->setBoundaries(terrainSize);

	SceneManager()->setCamera(m_pCamera);

	//Create a new terrain
	pTerrain = new Terrain(SceneManager());

	//or load
	if(FAILED(pTerrain->create(terrainSize)))
		return E_FAIL;

	pEditMenu = new EditMenu(pGUInterface->getMainElement());

	if(FAILED(pEditMenu->initialize()))
		return E_FAIL;

	bLButtonPressed = false;

	m_pSkydomeModel = std::auto_ptr<XMesh>(new XMesh(SceneManager()));
	m_pSkydomeModel->load("../data/meshes/skydome.x", RIGHTHANDED);

	Texture diffuseTex;
	SceneManager()->loadTexture(diffuseTex, "skydome_diffuse.dds");

	DX10Shader customShader = SceneManager()->getShaderSystem()->initShader("textureShader.fx", VERTEX);
	customShader.bindTexture(diffuseTex, "diffuseTex");

	Frame* pFrame = m_pSkydomeModel->getFrameByName("Sphere");
	pFrame->getModel()->setShader(customShader);
	
	SceneNode* pSkydomeNode = m_pSkydomeModel->getFrameByName("Root")->getNode();
	pSkydomeNode->translate(30.0f, 00.0f, 30.0f);
	//pSkydomeNode->rotate(1, 0, 0, -90 );//scale(5.0f, 5.0f, 5.0f);

	pSkydomeNode = pFrame->getNode();
	pSkydomeNode->setCastShadow(false);
	pSkydomeNode->setCastReflection(true);
	
	m_pTankModel = std::auto_ptr<XMesh>(new XMesh(SceneManager()));
	m_pTankModel->load("../data/meshes/tank.x", RIGHTHANDED);

	m_pTankModel->getFrameByName("Root")->getNode()->translate(5.0f, 0.0f, 15.0f);
	//m_pTankModel->getFrameByName("Root")->getNode()->setCastShadow(true);

	return S_OK;
}

void EditorState::handleEvent(Event& e)
{
	if(strcmp(e.getID(), "lbuttondown") == 0)
	{
		bLButtonPressed = true;
	}
	else if(strcmp(e.getID(), "lbuttonup") == 0)
	{
		bLButtonPressed = false;
	}
	if(strcmp(e.getID(), "mousemove") == 0)
	{		
		mousePos = e[0].toVector();
	}
}

void EditorState::render(float time)
{
	glm::vec3 toEye = - m_pCamera->getAt(); 
	float lenght      = glm::length(toEye);

	pTerrain->updateLod(m_pCamera->getAt() + glm::vec3(0.0f, m_pCamera->getEye().y, 0.0f));

	if(bLButtonPressed)
	{		
		glm::vec3 RayOrigin;
        glm::vec3 RayDirection;
        glm::vec3 HitPos;

		m_pCamera->calculateRay(mousePos, RayOrigin, RayDirection);

		if(pTerrain->intersect(RayOrigin, RayDirection, HitPos)) {
			switch(editorType.toInt()) {
				case 0:
					pTerrain->editTerrain(glm::vec2(HitPos.x, HitPos.z), time, editStrenght.toFloat(), 
                        editRadius.toFloat(), editType.toInt());
					break;
				case 1:
					pTerrain->editTexture(glm::vec2(HitPos.x, HitPos.z), time, editStrenght.toFloat(),
                        editRadius.toFloat(), editType.toInt());
					break;
			}
		}
	}
	
	pTerrain->update();
}

void EditorState::tick()
{	

}