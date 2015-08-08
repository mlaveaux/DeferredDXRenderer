#include <main/ConVar.h>
#include <main/GlobalFunctions.h>

#include "MenuState.h"


MenuState::MenuState()
{
	m_equatorangle = 0.0f;
}

MenuState::~MenuState()
{
	//Clean all objects created.
	pAudioSystem->stopSound(m_backgroundEmitter);

	SAFE_DELETE(m_pMainMenu);
	SAFE_DELETE(m_pPlanetModel);
}

int MenuState::Initialize()
{
	//Create and show the mainmenu
	m_pMainMenu = new MainMenu(pGUInterface->getMainElement());

	if(FAILED(m_pMainMenu->initialize()))
		return -1;

	ConVar r_windowSize("r_windowsize");

	//Create a camera
	SceneManager()->setCamera(new CameraNode(r_windowSize.toVector().x, r_windowSize.toVector().y, 65.0f));
	m_pCamera = SceneManager()->getCamera();
	m_pCamera->setLookAt(-8.0f, 0.0f, 0.0f);
	m_pCamera->setPosition(-8.0f, 0.0f, 19.0f);
	
	m_pPlanetModel = new XMesh(SceneManager());

	m_pPlanetModel->load("../data/meshes/planet.x", RIGHTHANDED);

	Material planetMaterial;
	SceneManager()->loadTexture(planetMaterial.diffuseTex, "planet.dds");

	m_pPlanetModel->getFrameByName("Sphere")->setMaterial(planetMaterial);
	m_pPlanetNode = m_pPlanetModel->getFrameByName("Sphere")->getNode();
	m_pPlanetNode->setCastShadow(false);
	
	//Load and play a sound
	m_backgroundMusic   = pAudioSystem->loadSound("background_music.wav");
	//m_backgroundEmitter = pAudioSystem->playSound(m_backgroundMusic, EMITTER_REPEAT);
		
	return 0;
}

void MenuState::render(float time)
{
	m_equatorangle += time / 30000;
	
	m_pPlanetNode->identity();

	float angle = 10.0f / 180.0f * 3.14f;
	
	m_pPlanetNode->rotate(0.0f, 0.0f, 1.0f, -angle);
//	m_pPlanetNode->rotate(::sinf(angle), ::cosf(angle), 0.0f, m_equatorangle);
	m_pPlanetNode->rotate(0.0f, 1.0f, 0.0f, -m_equatorangle);
}