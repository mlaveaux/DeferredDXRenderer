#include "DX11ShaderSystem.h"

#include "main/EventManager.h"
#include "main/ConVar.h"

ConVar r_ambientIntensity("r_ambientIntensity", CValue(0.35f), CVAR_FNONE, "");
ConVar r_ambientColor("r_ambientColor", CValue(0.35f), CVAR_FNONE, "");
ConVar r_dirLightPos("r_lightpos", CValue(0.0f, 1.0f, 0.0f), CVAR_FNONE, "");

DX11ShaderSystem::DX11ShaderSystem(DX11Context::Ptr context)
    : IEventListener("shadersystem"), m_context(context)
{}

DX11Shader DX11ShaderSystem::initShader(const char* strShaderName, VertexType type)
{
    UINT i = 0;
    for (i = 0; i < m_pEffectArray.size(); i++)
    {
        if (strcmp(m_pEffectArray[i].strName, strShaderName) == 0) {
            return DX11Shader(m_pEffectArray[i]);
        }
    }
    DX11Effect* pEffect = new DX11Effect();

    m_pEffectArray.push_back(pEffect);

    pEffect->indexId = m_pEffectArray.size();
    pEffect->strName = strFilename;

    DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
    dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

    char strPath[MAX_PATH];

    ::strcpy_s(strPath, MAX_PATH, "../data/effects/dx10deferred/");
    ::strcat_s(strPath, MAX_PATH, strFilename);

    ID3D10Blob* pError;
    HRESULT hr;

    if (FAILED(hr = D3DX11CreateEffectFromFile(strPath,
        NULL,
        NULL,
        "fx_4_0",
        dwShaderFlags,
        0,
        m_pd3dDevice,
        NULL,
        NULL,
        &pEffect->pEffect,
        &pError,
        NULL)))
    {
        if (pError != NULL)
        {
            char* strBuffer = (char*)pError->GetBufferPointer();
            return EventManager::Instance().postException(strBuffer, "");
        }
        else
            return EventManager::Instance().postException(hr, "Failed to load effect file: %s", strPath);
    }
    //pEffect->pEffect->Optimize();

    pEffect->pTechnique = pEffect->pEffect->GetTechniqueByIndex(0);
    if (!pEffect->pTechnique->IsValid())
        return EventManager::Instance().postException(hr, "Invalid technique in effect file: %s", strFilename);

    D3D10_PASS_DESC	PassDesc;

    pEffect->pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);

    UINT NumInputElements = 0;

    //switch(type)
    //{
    //case VERTEX:
    NumInputElements = 3;
    D3D10_INPUT_ELEMENT_DESC InputLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D10_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = m_pd3dDevice->CreateInputLayout(InputLayout,
        NumInputElements,
        PassDesc.pIAInputSignature,
        PassDesc.IAInputSignatureSize,
        &pEffect->pVertexLayout);

    return DX11Shader(m_pEffectArray.back());
}

void DX11ShaderSystem::handleEvent(Event& e)
{
    if (e.getID() == "cvar_change")
    {

    }
}
