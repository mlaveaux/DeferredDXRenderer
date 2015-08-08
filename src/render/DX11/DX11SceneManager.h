/* Copyright 2012-2015 Maurice Laveaux
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DX11SCENEMANAGER_H
#define DX11SCENEMANAGER_H

//Windows include for HRESULT

//Other includes
#include "render/vertextypes.h"
#include "render/CameraNode.h"
#include "DX11ShaderSystem.h"
#include "DX11Texture.h"
#include "DX11Global.h"
#include "main/globals.h"

//STL includes
#include <vector>
#include <memory>
#include <string>
#include <windows.h>

//Predefined classes and structs
class  SceneNode;
class  RenderModel;
struct DX11Mesh;

//The empty model is the first index.
#define EMPTY_MODEL 0

class DX11SceneManager
{
public:
    DX11SceneManager(ID3D10Device* pd3dDevice, DX11ShaderSystem* pShaderSyste);
    
	//RenderComponent related functions
	RenderModel* createModel();
	//Returns the pointer of a RenderModel
	RenderModel* getRenderModel(UINT index);

	//Update everything one frame
	void interpolate(float fTime);
	
	//CameraNode related functions
	void setCamera(CameraNode* pCamera);
	CameraNode* getCamera() { return m_pCamera; }
	
	//SceneNode related functions
	SceneNode* getMainNode() { return m_mainNode.get(); }
	SceneNode* createSceneNode(std::string strName, SceneNode* pParent);
	void       removeNode(SceneNode* pSceneNode);
	
	//Texture creation and manipulation functions.
	HRESULT createTexture(Texture& textureRef, std::string strName, UVector2 Size, TextureType type);
	HRESULT loadTexture(  Texture& textureRef, const char* strTexturename);
	//Returns the  array of a mappable dynamic texture
	BYTE*  map(Texture&   textureRef);
	void   unmap(Texture& textureRef);

	//Mesh functions
	HRESULT createMesh(DX11Mesh* pMesh, Vertex* pVertexArray, UINT uNumOfVertices, UINT* pIndexBuffer, UINT uNumOfIndices);

	void UpdateVertex(DX11Mesh& mesh, Vertex* pVertexArray, UINT Pos, UINT Size);
	void UpdateIndex(DX11Mesh& mesh , UINT* pIndexArray, UINT Pos, UINT Size);
	
	//Returns SceneGraph of the SceneNodes
	std::vector<SceneNode*>&     getSceneGraph();
	//Returns the ShaderSystem of this SceneManager.
	DX11ShaderSystem* getShaderSystem();

private:	
    ID3D11Device* m_pd3dDevice;
	DX11ShaderSystem* m_pShaderSystem;
	CameraNode* m_pCamera;

	//Highest node in the hierarchy
	std::unique_ptr<SceneNode> m_mainNode;

	//Generic default texture
	Texture defaultTexture;

	//Array of all textures.
	std::vector<std::shared_ptr<TextureData>>  m_pTextureArray;
	//Array of all scene nodes.
	std::vector<SceneNode>		m_pNodeArray;
	//Array of all 3DModels.
	std::vector<RenderModel> m_pModelArray;
};

#endif // DX11SCENEMANAGER_H