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

#ifndef DX11RENDERSYSTEM_H
#define DX11RENDERSYSTEM_H

//Global includes
#include "main/globals.h"
#include "main/EventManager.h"

//Basic includes for renderer
#include "render/Camera.h"
#include "render/RenderModel.h"
#include "render/vertextypes.h"

//Include all helper classes for RenderSystem.
#include "DX11Surface.h"
#include "DX11ShaderSystem.h"
#include "DX11SceneManager.h"
#include "DX11ParticleSystem.h"
#include "DX11Global.h"
#include "DX11Context.h"

//Windows include for HRESULT
#include <windows.h>

//DirectX11 includes
#include <math.h>
#include <dxgi.h>

//PreDefine debugElement since it is used in both ways
class DebugElement;

/**
 * Render sprites and meshes with a scene graph
 */
class DX11RenderSystem : public IEventListener
{
public:
    //Constructor
    DX11RenderSystem(DX11Context::Ptr context);
    ~DX11RenderSystem();

    //Init the DX11Device
    HRESULT initDevice(HWND hWindow);

    //Initialize a debugWindow
    HRESULT initDebug(DebugElement* pDebugElement);

    //Set the HWND of the window
    void setWindow(HWND hWindow);
    void handleEvent(Event& e);

    //Start a new frame
    void beginFrame(float fTime);

    //Display the frame
    void presentFrame(float fTime);

    DX11SceneManager* getSceneManager();
    DX11Surface*      getSurface();
private:
    //Render a SceneNode object using shaders.
    void renderNode(SceneNode* pNode);
    //Render a specific mesh, only VERTEX layout.
    void renderMesh(DX11Mesh& mesh);
    //Enable or disable alpha blending.
    void setBlendState(bool bEnabled);
    //Enable or disable depth stencil.
    void setDepthState(bool bEnabled);

    // create the directx device.
    HRESULT createSwapchain();
    HRESULT createReflectBuffer();
    HRESULT createPreProcessBuffer();
    //HRESULT createLinearDepthBuffer();
    HRESULT createShadowMap();
    HRESULT createBlendStates();

    // create both enabled and disabled depth states.
    int createDepthStates();

    // for ConVar changing purposes.
    void setViewports();
    void setRasterizerState(bool bWireframe);
    void setSwapchain();

    DX11SceneManager sceneManager;
    DX11ParticleSystem particleSystem;

    std::unique_ptr<DX11Surface> surface;
    std::unique_ptr<DX11ShaderSystem> shaderSystem;

    DebugElement* m_pDebugElement;


    //Device Helpers	
    IDXGISwapChain*	pSwapChain;

    //Render TargetView
    ID3D11RenderTargetView*	pRenderTargetView;

    ID3D11DepthStencilView*	pDepthStencilView;
    Texture depthTex;

    //preprocess targetview holds the color per pixel.
    ID3D11RenderTargetView* pPreProcessTargetView;
    Texture preProcessTex;

    //Depth texture holds the linear depth.
    //ID3D10RenderTargetView*	    pLinearDepthTargetView;
    //Texture                     linearDepthTex;

    //Reflection Texture
    ID3D11RenderTargetView*	pReflectionTargetView;
    Texture reflectTex;

    //Shadowmap Texture
    ID3D11DepthStencilView*	pShadowMapView;
    Texture shadowMapTex;

    //Shaders
    DX11Shader m_noColorShader;
    DX11Shader m_fogShader;
    DX11Mesh fullscreenMesh;

    //Variables
    D3D11_VIEWPORT viewPort[3];

    //Blend states enable/disable
    ID3D11BlendState* pBlendEnabled;
    ID3D11BlendState* pBlendDisabled;

    //Depth states enabled/disabled
    ID3D11DepthStencilState* m_pEnabledDepthState;
    ID3D11DepthStencilState* m_pDisabledDepthState;

    unsigned int m_uRenderCalls;
    unsigned int m_uPolycount;
    unsigned int m_uFramesPerSecond;

    HWND m_hWindow;

    float fTimeOld;
};

#endif // DX11RENDERSYSTEM_H