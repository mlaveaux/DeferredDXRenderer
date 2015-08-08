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

#include "DX11Global.h"
#include "DX11Context.h"
#include "DX11Global.h"
#include "main/ConVar.h"
#include "main/eventmanager.h"

ConVar r_perfHud("r_perfhud", CValue(false), CVAR_FNONE, "Enables NVIDIA(c) perfhud tools.");
ConVar r_debug("r_debug", CValue(false), CVAR_FNONE, "Enable directx sdk debug layer.");

DX11Context::DX11Context()
{
    // create swapchain and device
    IDXGIFactory* pFactory;
    IDXGIAdapter* pAdapter;

    HRESULT hr;

    // create a directx factory
    if (FAILED(hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory))) {
        postException(DXUtil::resolveError(hr), "Couldn't create the IDXGIFactory");
    }

    // take the primary adapter
    unsigned int index = 0;
    unsigned int adapterIndex = 0;
    bool bPerfAdapter = false;

    // if Perfhud is required search for a adapter.
    if (r_perfHud.toBool()) {
        while (pFactory->EnumAdapters(index, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
            DXGI_ADAPTER_DESC desc;

            if (SUCCEEDED(pAdapter->GetDesc(&desc))) {
                if (wcscmp(desc.Description, L"NVIDIA PerfHUD") == 0) {
                    adapterIndex = index;
                    bPerfAdapter = true;
                }
            }

            ++index;
        }
    }

    if (FAILED(hr = pFactory->EnumAdapters(adapterIndex, &pAdapter))) {
        postException(DXUtil::resolveError(hr), "The primary IDXGIAdapter is not valid.");
    }

    // fill the Swapchain description with the default/loaded settings
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_11_1
    }

    if (FAILED(hr = D3D11CreateDevice(pAdapter,
        D3D_DRIVER_TYPE_REFERENCE,
        NULL,
        NULL,
        featureLevels,
        sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL),
        D3D11_SDK_VERSION,
        &pd3dDevice,
        NULL,
        pd3dDeviceContext))) 
    {
        postException(DXUtil::resolveError(hr), "Failed to create DirectX11(c) device.");
    }

    postMessage("Created the DirectX11(c) device.");

    // read and display the videocard description.
    ::DXGI_ADAPTER_DESC AdapterDesc;
    pAdapter->GetDesc(&AdapterDesc);

    char strVideoCardDescription[128];

    wcstombs_s(NULL, strVideoCardDescription, 128, AdapterDesc.Description, 128);

    postMessage("Videocard          : %s", strVideoCardDescription);
    postMessage("Dedicated Memory   : %d MB", (unsigned int)AdapterDesc.DedicatedVideoMemory / 1024 / 1024);
    postMessage("Shared Memory      : %d MB", (unsigned int)AdapterDesc.SharedSystemMemory / 1024 / 1024);
}

DX11Context::~DX11Context()
{
    if (pd3dDeviceContext != NULL) {
        pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);
        pd3dDeviceContext->OMSetDepthStencilState(NULL, 0);
    }

    SAFE_RELEASE(pd3dDevice);
    SAFE_RELEASE(pd3dDeviceContext);
}

void DX11RenderSystem::setBlendState(bool bEnabled)
{
    const FLOAT fBlendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (bEnabled) {
        pd3dDeviceContext->OMSetBlendState(pBlendEnabled, fBlendFactor, 0xffffffff);
    }
    else {
        pd3dDeviceContext->OMSetBlendState(pBlendDisabled, fBlendFactor, 0xffffffff);
    }
}

void DX11RenderSystem::setDepthState(bool bEnabled)
{
    if (bEnabled) {
        pd3dDeviceContext->OMSetDepthStencilState(m_pEnabledDepthState, 0);
    }
    else {
        pd3dDeviceContext->OMSetDepthStencilState(m_pDisabledDepthState, 0);
    }
}