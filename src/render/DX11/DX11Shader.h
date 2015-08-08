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

#ifndef DX11SHADER_H
#define DX11SHADER_H

#include "DX11Global.h"
#include "DX11Texture.h"
#include "render/vertexTypes.h"
#include "main/EventManager.h"
#include "main/CVarManager.h"

#include <d3dx11effect.h>
#include <memory>

#define ENABLE_BLEND 1
#define ENABLE_LINEARDEPTH 2

struct DX11Effect
{
    unsigned int indexId;
    const char* strName;
    VertexType type;

    ID3DX11Effect*          pEffect;
    ID3DX11EffectTechnique* pTechnique;
    ID3D11InputLayout*	 pVertexLayout;
};

class ShaderBind
{
public:
    ShaderBind(DX11Effect* pEffect, std::string strName)
    {
        m_pEffect = pEffect;
        m_strName = strName;
    }

    std::string& getName() { return m_strName; }

    virtual void bind() = 0;
    virtual void unbind() = 0;
     
protected:
    DX11Effect* m_pEffect;

    //Name of the variable it binds to.
    std::string m_strName;
};

class DX11Shader
{
public:
    DX11Shader();
    DX11Shader(DX11Effect* pEffect);
    ~DX11Shader();

    //Sets the render pass
    void    setTechnique(UINT uPass);
    //Commits all shader binds.
    void    bindAll();
    void    unbindAll();

    void    setVector(const char* strName, float* pFloat);
    void    setVectorArray(const char* strName, float* pFloat, UINT offset, UINT count);

    void    setMatrix(const char* strName, glm::mat4x4 Matrix);
    //Binds engine object to shader resources
    void    bindTexture(Texture textureRef, const char* strName);
    void    bindTextureArray(Texture textureRefs[], const char* strName, unsigned int count);
    //Bind a constant value to a shader resource
    void    bindValue(CValue value, const char* strName);
    //Bind an existing convar to a shader resource
    void    bindCVar(const char* strConVar, const char* strName);

    void    setTextureArray(const char* strName, Texture textureRef, UINT uTexture);
    //Set various properties for the shader.
    void    setProperties(unsigned int uFlags);

    unsigned int getId()    { return m_pEffect->indexId; }
    unsigned int getFlags() { return m_uFlags; }
    //Returns the inputLayout used by the shader		
    ID3D11InputLayout* getInputLayout();

private:
    //Reference to the effect file
    std::unique_ptr<DX11Effect> m_pEffect;
    //Array of all bound variables
    std::vector<ShaderBind*> m_bindArray;
    //Flags of this shader
    unsigned int m_uFlags;
};

#endif // DX11SHADER_H