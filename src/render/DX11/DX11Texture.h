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

#ifndef DX11TEXTURE_H
#define DX11TEXTURE_H

#include "main/globalFunctions.h"
#include "DX11Global.h"

#include <string>
#include <memory>

enum LoadState
{
	STATE_STAGED,
	STATE_LOADED, 
};

enum TextureType
{
	TEXTURE_RGBA8,
	TEXTURE_NONE,
};

struct TextureData
{	
	TextureData()
	{
		m_strName    = "";
		m_loadState  = STATE_STAGED;
		m_pTexture2D      = NULL;
		m_pShaderResource = NULL;
	}

	~TextureData()
	{
		SAFE_RELEASE(m_pTexture2D);
		SAFE_RELEASE(m_pShaderResource);
	}

	//name of the texture.
	std::string                 m_strName;
	//DX11 texture data.
	ID3D11Texture2D*			m_pTexture2D;	
	//DX11 shader resource
	ID3D11ShaderResourceView* m_pShaderResource;
	//RowPitch
	UINT m_rowPitch;
	//Load state
	LoadState m_loadState;
};

class Texture
{
public:
	Texture() 
	{
		m_pData = NULL;
	}
	Texture(std::shared_ptr<TextureData> pData) { m_pData = pData; }
	~Texture()
	{

	}
	std::shared_ptr<TextureData>      getData() { return m_pData;  }

	LoadState getLoadState()
	{
		if(m_pData)
		{
			return m_pData->m_loadState;
		}
		else
			return STATE_STAGED;
	}
private:
	std::shared_ptr<TextureData> m_pData;
};

#endif