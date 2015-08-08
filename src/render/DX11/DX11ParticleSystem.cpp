#include "DX11ParticleSystem.h"

HRESULT DX11ParticleSystem::Initialize(DX11SceneManager* pSceneManager)
{
	m_pSceneManager = pSceneManager;
	m_pShaderSystem = pSceneManager->getShaderSystem();

//	particleShader = m_pShaderSystem->initShader("particleShader.fx", PARTICLE);
	
	return S_OK;
};

