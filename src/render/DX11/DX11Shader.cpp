#include "DX11Shader.h"

//********************************************************************************
//Shader resource bind classes used by DX11Shader
//********************************************************************************

//TextureBind class binds a texture to a shader variable.
class TextureBind : public ShaderBind
{
public:
    //Creates the ShaderBind class
    TextureBind(DX11Effect* pEffect, std::string strName) : ShaderBind(pEffect, strName)
    {
        m_ppShaderResources = NULL;
    }

    ~TextureBind()
    {
        SAFE_DELETE(m_textureRefs);
        SAFE_DELETE(m_ppShaderResources);
    }

    //Initializes the bind
    int init(Texture textureRef)
    {
        m_count = 1;
        m_textureRefs = new Texture[1];
        m_textureRefs[0] = textureRef;

        m_pShaderResource = m_pEffect->pEffect->GetVariableByName(m_strName.c_str())->AsShaderResource();
        if (!m_pShaderResource->IsValid()) {
            EventManager::Instance().postMessage("unknown shader variable %s in shader %s", m_strName.c_str(), m_pEffect->strName);
            return E_FAIL;
        }

        return 0;
    }

    int init(Texture textureRefs[], unsigned int count)
    {
        m_count = count;
        m_textureRefs = new Texture[m_count];
        for (unsigned int i = 0; i < m_count; ++i) {
            m_textureRefs[i] = textureRefs[i];
        }

        m_ppShaderResources = new ID3D11ShaderResourceView*[m_count];

        m_pShaderResource = m_pEffect->pEffect->GetVariableByName(m_strName.c_str())->AsShaderResource();
        if (!m_pShaderResource->IsValid()) {
            EventManager::Instance().postMessage("unknown shader variable %s in shader %s", m_strName.c_str(), m_pEffect->strName);
            return E_FAIL;
        }

        return 0;
    }

    //commit the bind to the device.
    void bind()
    {
        if (m_count == 1) {
            m_pShaderResource->SetResource(m_textureRefs[0].getData()->m_pShaderResource);
        }
        else {
            for (unsigned int i = 0; i < m_count; ++i) {
                m_ppShaderResources[i] = m_textureRefs[i].getData()->m_pShaderResource;
            }
            m_pShaderResource->SetResourceArray(m_ppShaderResources, 0, m_count);
        }
    }

    void unbind()
    {
        if (m_count == 1) {
            m_pShaderResource->SetResource(NULL);
        }
        else {
            for (unsigned int i = 0; i < m_count; ++i) {
                m_ppShaderResources[i] = NULL;
            }

            m_pShaderResource->SetResourceArray(m_ppShaderResources, 0, m_count);
        }
    }

private:
    unsigned int                          m_count;
    Texture*                              m_textureRefs;
    ID3D11ShaderResourceView**	      m_ppShaderResources;
    ID3DX11EffectShaderResourceVariable* m_pShaderResource;
};

//Binds a constant scalar to a shader variable.
class ScalarBind : public ShaderBind
{
public:
    ScalarBind(DX11Effect* pEffect, std::string strName) : ShaderBind(pEffect, strName) {}
    int init(CValue value)
    {
        m_value = value;
        m_pShaderResource = m_pEffect->pEffect->GetVariableByName(m_strName.c_str())->AsScalar();

        if (!m_pShaderResource->IsValid())
        {
            EventManager::Instance().postMessage("unknown shaderVariable %s in shader %s", m_strName.c_str(), m_pEffect->strName);
            return -1;
        }

        return 0;
    }

    void bind()
    {
        if (m_value.Type == T_INTEGER) {
            m_pShaderResource->SetInt(m_value.Integer);
        }
    }

    void unbind()
    {

    }

private:
    CValue m_value;
    ID3DX11EffectScalarVariable* m_pShaderResource;
};
//Bind a console variable to a shader variable.
class ConVarBind : public ShaderBind
{
public:
    ConVarBind(DX11Effect* pEffect, std::string strName) : ShaderBind(pEffect, strName) {}

    int init(std::string strName)
    {
        m_conVar = ConVar(strName.c_str());

        if (m_conVar.getType() == T_INTEGER || m_conVar.getType() == T_FLOAT)
        {
            m_pScalarResource = m_pEffect->pEffect->GetVariableByName(m_strName.c_str())->AsScalar();

            if (!m_pScalarResource->IsValid())
            {
                EventManager::Instance().postMessage("unknown shaderVariable %s in shader %s", m_strName.c_str(), m_pEffect->strName);
                return -1;
            }
        }
        else if (m_conVar.getType() == T_VECTOR_3D)
        {
            m_pVectorResource = m_pEffect->pEffect->GetVariableByName(m_strName.c_str())->AsVector();

            if (!m_pVectorResource->IsValid())
            {
                EventManager::Instance().postMessage("unknown shaderVariable %s in shader %s", m_strName.c_str(), m_pEffect->strName);
                return -1;
            }
        }

        return 0;
    }

    void bind()
    {
        if (m_conVar.getType() == T_INTEGER) {
            m_pScalarResource->SetInt(m_conVar.toInt());
        }
        else if (m_conVar.getType() == T_FLOAT) {
            m_pScalarResource->SetFloat(m_conVar.toFloat());
        }

        //if(m_conVar.getType() == T_VECTOR3d)
        //	m_pVectorResource->SetFloatVector(
    }

    void unbind()
    {

    }

private:
    ConVar m_conVar;
    ID3DX11EffectScalarVariable* m_pScalarResource;
    ID3DX11EffectVectorVariable* m_pVectorResource;
};

DX11Shader::DX11Shader()
{
    m_uFlags = 0;
    m_pEffect;
}

DX11Shader::DX11Shader(DX11Effect* pEffect)
{
    m_uFlags = 0;
    m_pEffect = pEffect;
}

DX11Shader::~DX11Shader()
{
    //for(int i = 0;i < m_bindArray.size();i++)
    //{
    //	SAFE_DELETE(m_bindArray[i]);
    //}

    //m_bindArray.clear();
}

void DX11Shader::setTechnique(UINT pass)
{
    m_pEffect->pTechnique->GetPassByIndex(pass)->Apply();
}

void DX11Shader::bindAll()
{
    for (UINT i = 0; i < m_bindArray.size(); ++i) {
        m_bindArray[i]->bind();
    }
}

void DX11Shader::unbindAll()
{
    for (UINT i = 0; i < m_bindArray.size(); ++i) {
        m_bindArray[i]->unbind();
    }
}

void DX11Shader::setVector(const char* strName, float* pFloat)
{
    ID3DX11EffectVectorVariable*		   pVector;

    pVector = m_pEffect->pEffect->GetVariableByName(strName)->AsVector();
    if (pVector->IsValid())
        pVector->SetFloatVector(pFloat);
    else
        EventManager::Instance().postMessage("Unknown variable: %s in shader: %s", strName, m_pEffect->strName);
}

void DX11Shader::setVectorArray(const char* strName, float* pFloat, UINT offset, UINT count)
{
    ID3DX11EffectVectorVariable*		   pVector;

    pVector = m_pEffect->pEffect->GetVariableByName(strName)->AsVector();
    if (pVector->IsValid())
        pVector->SetFloatVectorArray(pFloat, offset, count);
    else
        EventManager::Instance().postMessage("Unknown variable: %s in shader: %s", strName, m_pEffect->strName);
}

void DX11Shader::setMatrix(const char* strName, glm::mat4x4 Matrix)
{
    ID3DX11EffectMatrixVariable* pMatrixVariable;

    pMatrixVariable = m_pEffect->pEffect->GetVariableByName(strName)->AsMatrix();
    if (pMatrixVariable->IsValid()) {
        pMatrixVariable->SetMatrix(glm::value_ptr(Matrix));
    }
    else {
        EventManager::Instance().postMessage("Unknown matrix variable: %s in shader: %s", strName, m_pEffect->strName);
    }

}

void DX11Shader::bindTexture(Texture textureRef, const char* strName)
{
    for (UINT i = 0; i < m_bindArray.size(); ++i) {
        if (m_bindArray[i]->getName().compare(strName) == 0)
        {
            SAFE_DELETE(m_bindArray[i]);

            TextureBind* pTextureBind = new TextureBind(m_pEffect, strName);

            pTextureBind->init(textureRef);

            m_bindArray[i] = pTextureBind;

            return;
        }
    }

    TextureBind* pTextureBind = new TextureBind(m_pEffect, strName);

    pTextureBind->init(textureRef);

    m_bindArray.push_back(pTextureBind);
}

void DX11Shader::bindTextureArray(Texture* textureRefs, const char* strName, unsigned int count)
{
    for (UINT i = 0; i < m_bindArray.size(); i++)
    {
        if (m_bindArray[i]->getName().compare(strName) == 0)
        {
            SAFE_DELETE(m_bindArray[i]);

            TextureBind* pTextureBind = new TextureBind(m_pEffect.get(), strName);

            pTextureBind->init(textureRefs, count);

            m_bindArray[i] = pTextureBind;

            return;
        }
    }

    TextureBind* pTextureBind = new TextureBind(m_pEffect, strName);

    pTextureBind->init(textureRefs, count);

    m_bindArray.push_back(pTextureBind);
}

void DX11Shader::bindValue(CValue value, const char* strName)
{
    ScalarBind* pScalarBind = new ScalarBind(m_pEffect, strName);

    pScalarBind->init(value);

    m_bindArray.push_back(pScalarBind);
}

void DX11Shader::bindCVar(const char* strConVar, const char* strName)
{
    ConVarBind* pConVarBind = new ConVarBind(m_pEffect, strName);

    pConVarBind->init(strConVar);

    m_bindArray.push_back(pConVarBind);
}

void DX11Shader::setProperties(unsigned int uFlags)
{
    m_uFlags = uFlags;
}

void DX11Shader::setTextureArray(const char* strName, Texture texture, UINT uTexture)
{
    ID3DX11EffectShaderResourceVariable*  pTextureVariable;

    pTextureVariable = m_pEffect->pEffect->GetVariableByName(strName)->AsShaderResource();
    if (pTextureVariable->IsValid()) {
        pTextureVariable->SetResourceArray(&texture.getData()->m_pShaderResource, uTexture, 1);
    }
    else {
        EventManager::Instance().postMessage("Unknown shaderresource variable: %s in shader: %s", strName, m_pEffect->strName);
    }
}

ID3D11InputLayout* DX11Shader::getInputLayout()
{
    return m_pEffect->pVertexLayout;
}