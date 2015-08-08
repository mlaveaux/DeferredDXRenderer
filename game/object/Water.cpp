#include "water.h"

int Water::initialize(DX10SceneManager* pSceneManager)
{
	m_pSceneManager = pSceneManager;

	//Load textures
	m_pSceneManager->loadTexture(m_normalTex, "water/ocean_normal.dds");
	m_pSceneManager->loadTexture(m_depthTex, "depthTex");
	m_pSceneManager->loadTexture(m_reflectTex, "reflectionTexture");

	//Create Shader
	DX10Shader waterShader = m_pSceneManager->getShaderSystem()->initShader("oceanShader.fx", VERTEX);

	waterShader.bindTexture(m_normalTex, "normalTex");
	waterShader.bindTexture(m_depthTex, "depthTex");
	waterShader.bindTexture(m_reflectTex, "reflectTex");
	waterShader.setProperties(ENABLE_BLEND | ENABLE_LINEARDEPTH);
	
	//Create the renderModel
	m_waterModel = m_pSceneManager->createModel();
	m_waterModel->setShader(waterShader);	
	
	//Create Mesh
	Vertex pVertexArray[4];
	
    pVertexArray[0] = Vertex(glm::vec3(0.0f, 5.0f, 64.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    pVertexArray[1] = Vertex(glm::vec3(64.0f, 5.0f, 64.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    pVertexArray[2] = Vertex(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    pVertexArray[3] = Vertex(glm::vec3(64.0f, 5.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	m_waterModel->createMesh(pVertexArray, 4, NULL, 0);
    
	//Create scenenode
	m_sceneNode = m_pSceneManager->createSceneNode("ocean", m_pSceneManager->getMainNode());
	
	m_sceneNode->attachModel(m_waterModel->getID());

	return 0;
}