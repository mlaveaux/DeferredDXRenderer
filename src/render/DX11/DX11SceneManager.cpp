#include "DX11SceneManager.h"

#include "main/GlobalFunctions.h"

#include "render/RenderModel.h"
#include "render/SceneNode.h"

DX11SceneManager::DX11SceneManager(ID3D11Device* pd3dDevice, DX11ShaderSystem* pShaderSystem)
{
    //Sets the member references
    m_pd3dDevice = pd3dDevice;
    m_pShaderSystem = pShaderSystem;

	//Number one should be an invalid empty rendermodel
	m_pModelArray.push_back(RenderModel(0, NULL, NULL));

    //Loads the default texture to display when a texture is not found
    if (FAILED(loadTexture(defaultTexture, "default.dds"))) {
        return E_FAIL;
    }

    m_mainNode = SceneNode(this, NULL, "Scene01");
    m_pCamera = new CameraNode(0, 0, 0.0f);
}

RenderModel* DX11SceneManager::createModel()
{
	m_pModelArray.push_back(RenderModel(m_pModelArray.size(), this, m_pShaderSystem));

	return &m_pModelArray.back();
}

RenderModel* DX11SceneManager::getRenderModel(UINT uIndex)
{
	return &m_pModelArray[uIndex];
}

void DX11SceneManager::interpolate(float fTime)
{
	m_pCamera->interpolate(fTime);
}

void DX11SceneManager::setCamera(CameraNode* pCamera)
{
	m_pCamera = pCamera;
}

SceneNode* DX11SceneManager::createSceneNode(std::string strName, SceneNode* pParent)
{
    SceneNode node = SceneNode(this, pParent, strName);

    if (pParent != NULL) {
        pParent->addNode(node);
    }
    else {
        pParent->addNode(m_mainNode);
    }

    m_pNodeArray.push_back(node);

	return &m_pNodeArray.back();
}

void DX11SceneManager::removeNode(SceneNode* pSceneNode)
{
	for(UINT i = 0;i < m_pNodeArray.size();i++)
	{
		if((long)m_pNodeArray[i] == (long)pSceneNode)
		{
			m_pNodeArray.erase(m_pNodeArray.begin() + i);
		}
	}
}

HRESULT DX11SceneManager::createMesh(DX11Mesh* pMesh, Vertex* pVertexArray, UINT uNumOfVertices, UINT* pIndexArray, UINT uNumOfIndices)
{
	HRESULT hr;

	pMesh->Topology = D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED;
	pMesh->uNumOfVertices = uNumOfVertices;
	pMesh->uNumOfIndices  = uNumOfIndices;

	D3D10_BUFFER_DESC      BufferDescription;
	D3D10_SUBRESOURCE_DATA InitData;

	//*******************************************
	//Create the vertex buffer
	//*******************************************
	if(pVertexArray != NULL && uNumOfVertices > 0)
	{
		BufferDescription.Usage             = D3D10_USAGE_DEFAULT;
		BufferDescription.ByteWidth         = sizeof( Vertex ) * uNumOfVertices;
		BufferDescription.BindFlags         = D3D10_BIND_VERTEX_BUFFER;
		BufferDescription.CPUAccessFlags    = 0;	
		BufferDescription.MiscFlags         = 0;

		InitData.pSysMem           = pVertexArray;
		InitData.SysMemPitch       = sizeof( *pVertexArray );
		InitData.SysMemSlicePitch  = sizeof( *pVertexArray );
			
		hr = m_pd3dDevice->CreateBuffer(&BufferDescription,
									    &InitData,
									    &pMesh->pVertexBuffer ) ;
		if(FAILED(hr))
			return hr;
	}
	else
		return E_FAIL;

	//*******************************************
	//Create the index buffer or set the topology
	//*******************************************
	if(pIndexArray != NULL && uNumOfIndices > 0)
	{	
		BufferDescription.Usage				= D3D10_USAGE_DEFAULT;
		BufferDescription.ByteWidth         = sizeof( UINT ) * uNumOfIndices;
		BufferDescription.BindFlags			= D3D10_BIND_INDEX_BUFFER;
		BufferDescription.CPUAccessFlags    = 0;
		BufferDescription.MiscFlags         = 0;
		
		InitData.pSysMem = pIndexArray; 
		InitData.SysMemPitch       = sizeof( *pIndexArray );
		InitData.SysMemSlicePitch  = sizeof( *pIndexArray );
		
		hr = m_pd3dDevice->CreateBuffer(&BufferDescription,
									    &InitData, 
									    &pMesh->pIndexBuffer );

		if(FAILED(hr))
			return hr;
	}
	else
	{
		pMesh->Topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		pMesh->pIndexBuffer = NULL;
	}

	return S_OK;
}

HRESULT DX11SceneManager::createTexture(Texture& textureRef, std::string strName, UVector2 Size, TextureType type)
{	
	HRESULT hr;
	
	//Fill the data structure.
	D3D10_TEXTURE2D_DESC textureDesc;
	::ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width     = Size.x;
	textureDesc.Height    = Size.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format    = DXGI_FORMAT_R8G8B8A8_UNORM;

	textureDesc.SampleDesc.Count   = 1;
	textureDesc.SampleDesc.Quality = 0;

	textureDesc.Usage          = D3D10_USAGE_DYNAMIC;
	textureDesc.BindFlags      = D3D10_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	textureDesc.MiscFlags      = 0;    

	D3D10_SHADER_RESOURCE_VIEW_DESC textureSRDesc;
 
	textureSRDesc.Format					= textureDesc.Format;
	textureSRDesc.ViewDimension		        = D3D10_SRV_DIMENSION_TEXTURE2D;
	textureSRDesc.Texture2D.MipLevels       = 1;
	textureSRDesc.Texture2D.MostDetailedMip = 0;

	std::shared_ptr<TextureData> data(new TextureData());

	if(type != TEXTURE_NONE)
	{
		//Create a directx10 texture and make a shaderresource view of it.
		if(FAILED(hr = m_pd3dDevice->CreateTexture2D(&textureDesc, NULL, &data->m_pTexture2D)))
			return EventManager::Instance().postException(resolveError(hr), "ID3D10Device::CreateTexture2D() failed");

		if(FAILED(hr = m_pd3dDevice->CreateShaderResourceView(data->m_pTexture2D, &textureSRDesc, &data->m_pShaderResource)))
			return EventManager::Instance().postException(resolveError(hr), "ID3D10Device::CreateShaderResourceView() failed");
	}
	
	data->m_strName   = strName;
	data->m_loadState = STATE_LOADED;

	m_pTextureArray.push_back(data);

	textureRef = Texture(data);

	return S_OK;
}

HRESULT DX11SceneManager::loadTexture(Texture& textureRef, const char* strFilename)
{
	//Search the texturearray for another copy of the texture by name.
	for(UINT i = 0;i < m_pTextureArray.size(); i++)
	{
		if(m_pTextureArray[i]->m_pShaderResource)
		{
			//if names are equal the textures are.
			if(m_pTextureArray[i]->m_strName.compare(strFilename) == 0)
			{
				textureRef = Texture(m_pTextureArray[i]);

				return S_OK;
			}
		}
	}
		
	std::string strPath = "";
	strPath.append("..\\data\\textures\\");
	strPath.append(strFilename);

	HRESULT hr = S_OK;
	//Texture didn't exist so create a new one.
	std::shared_ptr<TextureData> data(new TextureData());
	//TextureInfo dependant on the settings of SceneManager.
	//D3DX11_IMAGE_LOAD_INFO info;
	//::ZeroMemory(&info, sizeof(info));
	//info.BindFlags = D3D10_BIND_SHADER_RESOURCE;

	//Try to load the texture and create a shadersource view from a file.
	if(FAILED(hr = D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, strPath.c_str(), NULL, NULL, &data->m_pShaderResource, NULL)))
	{
		EventManager::Instance().postError(hr, "Loading texture: %s failed", strPath.c_str());
			
		//if there is no first index of the texture array.
		//Default.png doesn't exist then.
		if(m_pTextureArray.size() > 0)
			textureRef = Texture(m_pTextureArray[0]);
		else
			return EventManager::Instance().postException(resolveError(hr), "Loading the default texture: %s failed", strPath.c_str());
		
		//Although the loading failed, the texture still exist as a generic
		//default.png version (PURPLE/WHITE boxes)
		return hr;
	}

	data->m_strName   = strFilename;
	data->m_loadState = STATE_LOADED;

	m_pTextureArray.push_back(data);

	textureRef = Texture(data);

	return S_OK;
}

BYTE* DX11SceneManager::map(Texture& textureRef)
{
	D3D10_MAPPED_TEXTURE2D data; 

	HRESULT hr = textureRef.getData()->m_pTexture2D->Map(::D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &data);
	textureRef.getData()->m_rowPitch = (UINT)((float)data.RowPitch);

	return (BYTE*)data.pData;
}

void DX11SceneManager::unmap(Texture& textureRef)
{
	textureRef.getData()->m_pTexture2D->Unmap(::D3D10CalcSubresource(0, 0, 1));
}

void DX11SceneManager::UpdateVertex(DX11Mesh& mesh, Vertex* pVertexArray, UINT Offset, UINT Size)
{
	D3D10_BOX Box;
	Box.left  = sizeof(Vertex) * Offset;
	Box.right = sizeof(Vertex) * (Offset + Size);
	Box.front = 0;
	Box.back  = 1;
	Box.top   = 0;
	Box.bottom = 1;

	UINT uRowPitch = sizeof(Vertex) * Size;
	
	m_pd3dDevice->UpdateSubresource(mesh.pVertexBuffer, 0, &Box, pVertexArray, uRowPitch, 0);
}

void DX11SceneManager::UpdateIndex(DX11Mesh& mesh, UINT* pIndexArray, UINT Offset, UINT Size)
{
	D3D10_BOX Box;
	Box.left  = sizeof(UINT) * Offset;
	Box.right = sizeof(UINT) * (Offset + Size);
	Box.front = 0;
	Box.back  = 1;
	Box.top   = 0;
	Box.bottom = 1;

	UINT uRowPitch = sizeof(UINT) * Size;

	m_pd3dDevice->UpdateSubresource(mesh.pIndexBuffer, 0, &Box, pIndexArray, uRowPitch, 0);
}

std::vector<SceneNode>& DX11SceneManager::getSceneGraph()
{
	glm::mat4x4 identity(1.0f);

	m_mainNode->update(identity);

	return m_pNodeArray;
}

DX11ShaderSystem* DX11SceneManager::getShaderSystem()
{
	return m_pShaderSystem;
}