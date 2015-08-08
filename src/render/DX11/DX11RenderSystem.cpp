#include "DX11RenderSystem.h"

#include "main/GlobalFunctions.h"
#include "main/EventManager.h"
#include "main/ConVar.h"
#include "render/SceneNode.h"
#include "render/DebugElement.h"

#include <glm/gtc/type_ptr.hpp>

//Render specific cvars, see description for details
ConVar r_WindowSize("r_windowsize", CValue(800, 600), CVAR_FNONE, "size of the main rendertarget(resolution)");
ConVar r_ReflectMap("r_reflectmap", CValue(800, 600), CVAR_FNONE, "size of the reflectmap");
ConVar r_ShadowSize("r_shadowmapsize", CValue(1024, 1024), CVAR_FNONE, "size of the shadowmap");
ConVar r_RefreshRate("r_refreshrate", CValue(60), CVAR_FNONE, "targeted refreshrate");
ConVar r_MultiSamples("r_multisamples", CValue(1), CVAR_FNONE, "set the number of multisamples");
ConVar r_AntiAliasing("r_antialiasing", CValue(1), CVAR_FNONE, "set the anti aliasing quality");
ConVar r_FieldOfView("r_fieldofview", CValue(90.0f), CVAR_FNONE, "vertical field of view");
ConVar r_HDRlighting("r_hdrlighting", CValue(true), CVAR_FNONE, "enables high dynamic range lighting");
ConVar r_Fullscreen("r_fullscreen", CValue(false), CVAR_FNONE, "enables fullscreen");
ConVar r_Brightness("r_brightness", CValue(50), CVAR_FNONE, "");
ConVar r_Wireframe("r_wireframe", CValue(false), CVAR_FNONE, "");

//Default after effect specific settings, custom after effects not yet supported.
ConVar r_postProcess("r_postprocess", CValue(true), CVAR_FNONE, "");
ConVar r_Fog("r_fog", CValue(true), CVAR_FNONE, "");
ConVar r_Bloom("r_bloom", CValue(true), CVAR_FNONE, "");
ConVar r_Reflection("r_reflection", CValue(true), CVAR_FNONE, "");
ConVar r_ShadowMap("r_shadowmap", CValue(true), CVAR_FNONE, "");

ConVar g_fogStart("g_fogstart", CValue(10), CVAR_FNONE, "");
ConVar g_fogEnd("g_fogend", CValue(200), CVAR_FNONE, "");

DX11RenderSystem::DX11RenderSystem(DX11Context::Ptr context) 
    : IEventListener("renderer"), m_context(context)
{
    //Initialize the helper classes of the RenderSystem
    shaderSystem = std::unique_ptr<DX11ShaderSystem>(new DX11ShaderSystem(context));

    fTimeOld = 0;
    m_uFramesPerSecond = 0;
    m_uPolycount = 0;
    m_uRenderCalls = 0;

    m_pDebugElement = NULL;

    pd3dDevice->CreateDeferredContext(NULL, &pd3dDeviceContext);
}

DX11RenderSystem::~DX11RenderSystem()
{

    SAFE_RELEASE(pSwapChain);
    SAFE_RELEASE(pDepthStencilView);
    SAFE_RELEASE(pShadowMapView);
}

HRESULT DX11RenderSystem::initDevice(HWND hWindow)
{
    m_hWindow = hWindow;
    

    if (FAILED(sceneManager.initialize(pd3dDevice, &shaderSystem))) {
        return E_FAIL;
    }

    surface = std::unique_ptr<DX11Surface>(new DX11Surface(pd3dDevice, sceneManager));

    if (FAILED(particleSystem.Initialize(&sceneManager)))
        return E_FAIL;

    //Initializes the core directx10(c) functionality.

    //The preprocessing buffer, filled with color
    if (FAILED(createPreProcessBuffer()))
        return E_FAIL;

    //Create a linear depth buffer, because logarithmic depth sucks.
    //if(FAILED(createLinearDepthBuffer()))
    //	return E_FAIL;

    //The reflection buffer according to r_reflectionsize
    if (FAILED(createReflectBuffer()))
        return E_FAIL;
    //Shadowmap texture according to r_shadowmapsize
    if (FAILED(createShadowMap()))
        return E_FAIL;
    //Blend states ( Enabled | Disabled )
    if (FAILED(createBlendStates()))
        return E_FAIL;
    //Create depth states ( Enabled | Disabled )
    if (createDepthStates() != 0)
        return E_FAIL;

    // makes everything ready to start rendering.
    setRasterizerState(false);
    setViewports();

    // shader to render objects to the shadowmap.
    m_noColorShader = shaderSystem.initShader("nocolorShader.fx", VERTEX);

    // after effects.
    m_fogShader = shaderSystem.initShader("fogShader.fx", SPRITE);
    Texture texArray[] = { preProcessTex, depthTex };
    m_fogShader.bindTextureArray(texArray, "inputTex", 2);

    m_fogShader.bindCVar("g_fogstart", "fogStart");
    m_fogShader.bindCVar("g_fogend", "fogEnd");

    Vertex pVertexArray[4];

    pVertexArray[0] = Vertex(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    pVertexArray[1] = Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    pVertexArray[2] = Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    pVertexArray[3] = Vertex(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    sceneManager.createMesh(&fullscreenMesh, pVertexArray, 4, NULL, 0);

    return S_OK;
}

HRESULT DX11RenderSystem::initDebug(DebugElement* pDebugElement)
{
    //Create a debug window to display FPS and drawcalls.
    m_pDebugElement = pDebugElement;

    if (FAILED(m_pDebugElement->initialize())) {
        return E_FAIL;
    }

    return S_OK;
}

void DX11RenderSystem::handleEvent(Event& e)
{
    if (e.getID() == "cvar_change")
    {
        //if(e[0].toLong() == r_Wireframe.getID())
        //	setRasterizerState();
    }
}

void DX11RenderSystem::beginFrame(float fTime)
{
    // clear the frame
    pd3dDeviceContext->ClearRenderTargetView(pRenderTargetView, glm::value_ptr(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));

    // calculate the view frustum
    glm::vec3 viewFrustum[8];

    //Get the active camera
    CameraNode* mainCamera = sceneManager.getCamera();

    //Get the far and near place sizes
    glm::mat4x4  viewMatrix = mainCamera->getViewMatrix();
    glm::vec3 viewPos = mainCamera->getEye();
    glm::vec3 atPos = mainCamera->getAt();

    float viewDistance = glm::length(viewPos - atPos) * 1.5f;
    viewDistance = std::min(viewDistance, 10.0f);

    glm::vec2 farSize;
    glm::vec2 nearSize;

    float aspectRatio = (float)viewPort[0].Width / (float)viewPort[0].Height;

    farSize.y = 2.0f * tanf(r_FieldOfView.toFloat() * 0.5f) * viewDistance;
    farSize.x = farSize.y * aspectRatio;

    nearSize.y = 2.0f * tanf(r_FieldOfView.toFloat() * 0.5f) * 1.0f;
    nearSize.x = nearSize.y * aspectRatio;

    glm::vec3 viewRight = glm::vec3(viewMatrix._11, viewMatrix._21, viewMatrix._31);
    glm::vec3 viewUp = glm::vec3(viewMatrix._12, viewMatrix._22, viewMatrix._32);
    glm::vec3 viewDirection = glm::vec3(viewMatrix._13, viewMatrix._23, viewMatrix._33);

    //4 points of the far plane
    glm::vec3 farPos = viewPos + viewDirection * viewDistance;
    viewFrustum[0] = farPos + (viewUp * farSize.y * 0.5f) - (viewRight * farSize.x * 0.5f);
    viewFrustum[1] = farPos + (viewUp * farSize.y * 0.5f) + (viewRight * farSize.x * 0.5f);
    viewFrustum[2] = farPos - (viewUp * farSize.y * 0.5f) - (viewRight * farSize.x * 0.5f);
    viewFrustum[3] = farPos - (viewUp * farSize.y * 0.5f) + (viewRight * farSize.x * 0.5f);

    //4 points of the near plane
    glm::vec3 nearPos = viewPos + viewDirection * 1.0f;
    viewFrustum[4] = nearPos + (viewUp * nearSize.x * 0.5f) - (viewRight * nearSize.y * 0.5f);
    viewFrustum[5] = nearPos + (viewUp * nearSize.x * 0.5f) + (viewRight * nearSize.y * 0.5f);
    viewFrustum[6] = nearPos - (viewUp * nearSize.x * 0.5f) - (viewRight * nearSize.y * 0.5f);
    viewFrustum[7] = nearPos - (viewUp * nearSize.x * 0.5f) + (viewRight * nearSize.y * 0.5f);

    sceneManager.interpolate(fTime);

    std::vector<SceneNode> pBufferArray = sceneManager.getSceneGraph();

    // calculate the light frustum
    glm::vec3 centerPos = mainCamera->getAt();

    for (UINT i = 0; i < 8; ++i)
    {
        centerPos += viewFrustum[i];
    }
    centerPos /= 8;

    glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);
    lightPos = glm::normalize(lightPos);

    glm::mat4x4 lightViewMatrix;
    glm::mat4x4 lightProjMatrix;

    lightPos = centerPos + lightPos * viewDistance;
    lightViewMatrix = glm::lookAt(lightPos, centerPos, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec4 lightFrustum[8];

    for (UINT i = 0; i < 8; ++i) {
        lightFrustum[i] = glm::vec4(viewFrustum[i], 0.0f) * lightViewMatrix;
    }

    float minX = lightFrustum[0].x;
    float maxX = lightFrustum[0].x;
    float minY = lightFrustum[0].y;
    float maxY = lightFrustum[0].y;
    float minZ = lightFrustum[0].z;
    float maxZ = lightFrustum[0].z;

    for (UINT i = 1; i < 8; i++)
    {
        if (lightFrustum[i].x < minX) {
            minX = lightFrustum[i].x;
        }

        if (lightFrustum[i].x > maxX) {
            maxX = lightFrustum[i].x;
        }

        if (lightFrustum[i].y < minY) {
            minY = lightFrustum[i].y;
        }

        if (lightFrustum[i].y > maxY) {
            maxY = lightFrustum[i].y;
        }

        if (lightFrustum[i].z < minZ) {
            minZ = lightFrustum[i].z;
        }

        if (lightFrustum[i].z > maxZ) {
            maxZ = lightFrustum[i].z;
        }
    }

    lightProjMatrix = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    glm::mat4x4 lightViewProjMatrix = lightViewMatrix * lightProjMatrix;

    // render the reflection target
    setBlendState(false);
    setDepthState(true);

    //Clear the depth and reflection targets
    pd3dDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);
    pd3dDeviceContext->ClearRenderTargetView(pReflectionTargetView, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));

    //Set the reflection target
    pd3dDeviceContext->OMSetRenderTargets(1, &pReflectionTargetView, pDepthStencilView);
    pd3dDeviceContext->RSSetViewports(1, &viewPort[2]);
    pd3dDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Invert the camera in a plane lying at z=waterheight, with a normal point up.
    viewMatrix = glm::lookAt(glm::vec3(viewPos.x, 5.0 - (viewPos.y - 5.0f), viewPos.z),
        glm::vec3(atPos.x, 5.0f - (atPos.y - 5.0f), atPos.z), 
        glm::vec3(0, -1, 0));

    glm::mat4x4 reflectViewProjMatrix = viewMatrix * mainCamera->getProjMatrix();

    std::array<glm::vec3, 4> boundingBox;

    glm::mat4x4 world;

    for (UINT i = 0; i < pBufferArray.size(); i++)
    {
        SceneNode* pNode = pBufferArray[i];

        RenderModel* pModel = sceneManager.getRenderModel(pNode->getModelID());

        if (pModel->getID() != EMPTY_MODEL && pNode->isVisible() && pNode->castReflection())
        {
            //check frustum vs boundingbox
            boundingBox = pModel->getBoundingBox();
            world = pModel->getMatrix() * pNode->getMatrix();

            //Sets the shaders bind and input layout
            DX11Shader shader = pModel->getShader();

            //Set the shader variables
            shader.setVector("cameraPos", glm::value_ptr(viewPos));
            shader.setMatrix("worldViewProj", world * reflectViewProjMatrix);
            shader.setMatrix("world", world);

            shader.bindAll();
            //Set the technique to be used			
            shader.setTechnique(0);
            //Render the model
            pd3dDeviceContext->IASetInputLayout(shader.getInputLayout());
            renderMesh(pModel->getMesh());

            m_uRenderCalls++;
        }
    }

    // render the shadowMap depth, without color
    pd3dDeviceContext->ClearDepthStencilView(pShadowMapView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    pd3dDeviceContext->OMSetRenderTargets(0, NULL, pShadowMapView);
    pd3dDeviceContext->RSSetViewports(1, &viewPort[1]);
    pd3dDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (UINT i = 0; i < pBufferArray.size(); i++)
    {
        SceneNode* pNode = pBufferArray[i];

        RenderModel* pModel = sceneManager.getRenderModel(pNode->getModelID());

        if (pModel->getID() != EMPTY_MODEL && pNode->isVisible() && pNode->castShadow())
        {
            m_noColorShader.setMatrix("lightViewProj", pModel->getMatrix() * pNode->getMatrix() * lightViewProjMatrix);
            m_noColorShader.setTechnique(0);

            pd3dDeviceContext->IASetInputLayout(m_noColorShader.getInputLayout());
            renderMesh(pModel->getMesh());

            m_uRenderCalls++;
        }
    }
    // Sort the render buffer on shader properties, using countingsort.
    // The sub-array A[0..numOfOpaque-1] contains all shaders with flag 0.
    // The sub-array A[numOfOpaque..numOfSolids+numOfTransparent-1] contains all shaders with flag ENABLE_BLEND.
    // The sub-rray A[numOfOpaque+numOfTransparent..size-1] contains all the shaders with flags ENABLE_BLEND | ENABLE_LINEARDEPTH.
    unsigned int uFlags;
    unsigned int numOfOpaque = 0;
    unsigned int numOfTransparent = 0;
    unsigned int numOfDepth = 0;

    //Count the size of each subarray
    for (unsigned int i = 0; i < pBufferArray.size(); i++)
    {
        SceneNode* pNode = pBufferArray[i];

        RenderModel* pModel = sceneManager.getRenderModel(pNode->getModelID());

        if (pModel->getID() != EMPTY_MODEL && pNode->isVisible())
        {
            //get the shader flags
            DX11Shader shader = pModel->getShader();
            uFlags = shader.getFlags();
            //Count for each option the number of items
            if (uFlags == 0)
            {
                numOfOpaque++;
            }
            if (uFlags == ENABLE_BLEND)
            {
                ++numOfTransparent;
            }
            if (uFlags == (ENABLE_BLEND | ENABLE_LINEARDEPTH))
            {
                ++numOfDepth;
            }
        }
    }

    unsigned int indexOpaque = 0;
    unsigned int indexTransparent = numOfOpaque;
    unsigned int indexDepth = numOfOpaque + numOfTransparent;

    //Now exchange the items in the array until there are 3 subarrays with only 1 flag possibility.
    for (unsigned int i = 0; i < pBufferArray.size(); ++i)
    {
        SceneNode* pNode = pBufferArray[i];

        RenderModel* pModel = sceneManager.getRenderModel(pNode->getModelID());

        if (pModel->getID() != EMPTY_MODEL && pNode->isVisible())
        {
            //get the shader flags
            DX11Shader shader = pModel->getShader();
            uFlags = shader.getFlags();

            if (uFlags == 0)
            {
                pBufferArray[i] = pBufferArray[indexOpaque];
                pBufferArray[indexOpaque] = pNode;

                indexOpaque++;
            }
            if (uFlags == ENABLE_BLEND)
            {
                pBufferArray[i] = pBufferArray[indexTransparent];
                pBufferArray[indexTransparent] = pNode;

                indexTransparent++;
            }
            if (uFlags == (ENABLE_BLEND | ENABLE_LINEARDEPTH))
            {
                pBufferArray[i] = pBufferArray[indexDepth];
                pBufferArray[indexDepth] = pNode;

                indexDepth++;
            }
        }
    }

    // render the main renderTarget(s)
    pd3dDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);
    pd3dDeviceContext->ClearRenderTargetView(pPreProcessTargetView, D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.0f));

    //Determine which rendertarget to use
    if (r_postProcess.toBool()) {
        //ID3D10RenderTargetView* ppRenderTarget[] = { pPreProcessTargetView, pLinearDepthTargetView }; 
        //pd3dDevice->OMSetRenderTargets(2, ppRenderTarget, pDepthStencilView);
        pd3dDeviceContext->OMSetRenderTargets(1, &pPreProcessTargetView, pDepthStencilView);
    }
    else {
        pd3dDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
    }

    //Set the size of the viewport
    pd3dDeviceContext->RSSetViewports(1, &viewPort[0]);

    glm::mat4x4 viewProjMatrix = mainCamera->getViewMatrix() * mainCamera->getProjMatrix();

    //Render all solid/opaque objects
    for (unsigned int i = 0; i < numOfOpaque; ++i) {
        SceneNode* pNode = pBufferArray[i];

        RenderModel* pModel = sceneManager.getRenderModel(pNode->getModelID());

        if (pModel->getID() != EMPTY_MODEL && pNode->isVisible())
        {
            //check frustum vs boundingbox
            boundingBox = pModel->getBoundingBox();
            world = pModel->getMatrix() * pNode->getMatrix();

            //Sets the shaders bind and input layout
            DX11Shader shader = pModel->getShader();

            //Set the shader variables
            shader.setVector("cameraPos", glm::value_ptr(viewPos));
            shader.setMatrix("lightViewProj", world * lightViewProjMatrix);
            shader.setMatrix("worldViewProj", world * viewProjMatrix);
            shader.setMatrix("world", world);

            shader.bindAll();
            //Set the technique to be used			
            shader.setTechnique(0);
            //Render the model
            pd3dDeviceContext->IASetInputLayout(shader.getInputLayout());
            renderMesh(pModel->getMesh());

            ++m_uRenderCalls;
        }
    }

    // enable alpha blending and render transparent objects
    setBlendState(true);

    for (unsigned i = numOfOpaque; i < numOfOpaque + numOfTransparent; ++i) {
        SceneNode* pNode = pBufferArray[i];
        RenderModel* pModel = sceneManager.getRenderModel(pNode->getModelID());

        if (pModel->getID() != EMPTY_MODEL && pNode->isVisible()) {
            //check frustum vs boundingbox
            boundingBox = pModel->getBoundingBox();
            world = pModel->getMatrix() * pNode->getMatrix();

            //Sets the shaders bind and input layout
            DX11Shader shader = pModel->getShader();

            //Set the shader variables
            shader.setVector("cameraPos", glm::value_ptr(viewPos));
            shader.setMatrix("lightViewProj", world * lightViewProjMatrix);
            shader.setMatrix("worldViewProj", world * viewProjMatrix);
            shader.setMatrix("world", world);

            shader.bindAll();
            //Set the technique to be used			
            pd3dDeviceContext->IASetInputLayout(shader.getInputLayout());
            shader.setTechnique(0);
            //Render the model
            renderMesh(pModel->getMesh());

            m_uRenderCalls++;
        }
    }

    // Enable alpha blending and disable linearDepth write, then draw depthRequiring objects
    //setBlendState(true);
    setDepthState(false);

    pd3dDeviceContext->OMSetRenderTargets(1, &pPreProcessTargetView, NULL);

    for (unsigned i = numOfOpaque; i < pBufferArray.size(); ++i) {
        SceneNode* pNode = pBufferArray[i];
        RenderModel* pModel = sceneManager.getRenderModel(pNode->getModelID());

        if (pModel->getID() != EMPTY_MODEL && pNode->isVisible()) {
            //check frustum vs boundingbox
            boundingBox = pModel->getBoundingBox();
            world = pModel->getMatrix() * pNode->getMatrix();

            //Sets the shaders bind and input layout
            DX11Shader shader = pModel->getShader();

            glm::mat4x4 projMatrix = mainCamera->getProjMatrix();
            glm::vec2 projVector = glm::vec2(projMatrix._43, projMatrix._33);

            shader.setVector("proj", glm::value_ptr(projVector));

            //Set the shader variables
            shader.setVector("cameraPos", glm::value_ptr(viewPo)s);
            shader.setMatrix("reflectViewProj", world * reflectViewProjMatrix);
            shader.setMatrix("lightViewProj", world * lightViewProjMatrix);
            shader.setMatrix("worldViewProj", world * viewProjMatrix);
            shader.setMatrix("world", world);

            shader.bindAll();
            //Set the technique to be used			
            pd3dDeviceContext->IASetInputLayout(shader.getInputLayout());
            shader.setTechnique(0);
            //Render the model
            renderMesh(pModel->getMesh());

            shader.unbindAll();
            shader.setTechnique(0);

            m_uRenderCalls++;
        }
    }

    // enable alpha blend and render all sprites	
    //setDepthState(false);

    if (r_Wireframe.toBool()) {
        setRasterizerState(false);
    }

    pd3dDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);

    // fullscreen after effects 
    if (r_postProcess.toBool()) {
        pd3dDeviceContext->IASetInputLayout(m_fogShader.getInputLayout());

        m_fogShader.bindAll();

        glm::mat4x4 projMatrix = mainCamera->getProjMatrix();
        glm::vec2 projVector = glm::vec2(projMatrix._43, projMatrix._33);

        m_fogShader.setVector("proj", glm::value_ptr(projVector));
        m_fogShader.setTechnique(0);

        renderMesh(fullscreenMesh);

        m_fogShader.unbindAll();
        m_fogShader.setTechnique(0);
    }
}

void DX11RenderSystem::presentFrame(float fTime)
{
    //Show debug info
    if (m_pDebugElement != NULL) {
        //m_uRenderCalls += Surface.getRenderCalls();

        m_pDebugElement->setRenderCalls(m_uRenderCalls);
        m_pDebugElement->setPolycount(m_uPolycount);
    }

    //If 1 second has passed we display the FPS.
    if (fTimeOld >= 1000) {
        if (m_pDebugElement != NULL) {
            m_pDebugElement->setFramesPerSecond(m_uFramesPerSecond);
        }
        m_uFramesPerSecond = 0;

        fTimeOld = 0;
    }

    //Reset the renderCalls counter.
    m_uRenderCalls = 0;
    m_uPolycount = 0;
    //Increment the FPS.
    m_uFramesPerSecond++;

    fTimeOld += fTime;

    //Present the frame
    if (r_RefreshRate.toInt() > 0) {
        pSwapChain->Present(1, 0);
    }
    else
        pSwapChain->Present(0, 0);
}

DX11SceneManager* DX11RenderSystem::getSceneManager()
{
    return &sceneManager;
}

DX11Surface* DX11RenderSystem::getSurface()
{
    return &Surface;
}

//Private functions
void DX11RenderSystem::renderMesh(DX11Mesh& mesh)
{
    UINT Stride = sizeof(Vertex);
    UINT Offset = 0;

    if (mesh.Topology > 0)
    {
        pd3dDevice->IASetPrimitiveTopology(mesh.Topology);
    }
    else
    {
        pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        pd3dDevice->IASetIndexBuffer(mesh.pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    }

    pd3dDevice->IASetVertexBuffers(0, 1, &mesh.pVertexBuffer, &Stride, &Offset);

    if (mesh.Topology > 0)
        pd3dDevice->Draw(mesh.uNumOfVertices, 0);
    else
        pd3dDevice->DrawIndexed(mesh.uNumOfIndices, 0, 0);

    m_uPolycount += (unsigned int)((float)mesh.uNumOfIndices* 0.33f);
}

HRESULT DX11RenderSystem::createSwapchain()
{
    // create the swapchain.
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));

    sd.BufferCount = 2;
    sd.BufferDesc.Width = r_WindowSize.toVector().x;
    sd.BufferDesc.Height = r_WindowSize.toVector().y;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    sd.BufferDesc.RefreshRate.Numerator = r_RefreshRate.toInt();
    sd.BufferDesc.RefreshRate.Denominator = 1;

    UINT quality;
    pd3dDevice->CheckMultisampleQualityLevels(sd.BufferDesc.Format, r_MultiSamples.toInt(), &quality);

    sd.SampleDesc.Count = r_MultiSamples.toInt();
    sd.SampleDesc.Quality = quality - 1;

    sd.OutputWindow = m_hWindow;
    sd.Windowed = (!r_Fullscreen.toBool());

    if (FAILED(hr = pFactory->CreateSwapChain(pd3dDevice, &sd, &pSwapChain)))
        return postException(resolveError(hr), "D3D10CreateSwapChain() failed");

    // create the rendertargetview from the swapchain
    ID3D11Texture2D* pRenderTarget;
    hr = pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (void**)&pRenderTarget);
    if (FAILED(hr))
        return postException(resolveError(hr), "IDXGISwapChain::GetBuffer() failed");

    if (FAILED(hr = pd3dDevice->CreateRenderTargetView(pRenderTarget, NULL, &pRenderTargetView)))
        return postException(resolveError(hr), "ID3D10Device::CreateRenderTargetView() failed");

    // create the depthBuffer
    sceneManager.createTexture(depthTex, "depthTex", UVector2(0, 0), TEXTURE_NONE);

    D3D11_TEXTURE2D_DESC depthBufferDesc;
    depthBufferDesc.Width = r_WindowSize.toVector().x;
    depthBufferDesc.Height = r_WindowSize.toVector().y;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;

    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;

    depthBufferDesc.Usage = D3D10_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    D3D11_SHADER_RESOURCE_VIEW_DESC depthTexSRDesc;
    depthTexSRDesc.Format = DXGI_FORMAT_R32_FLOAT;
    depthTexSRDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    depthTexSRDesc.Texture2D.MipLevels = 1;
    depthTexSRDesc.Texture2D.MostDetailedMip = 0;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // create the depthstencil and the depthstencil view and shaderresource
    if (FAILED(hr = pd3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &depthTex.getData()->m_pTexture2D))) {
        return postException(resolveError(hr), "ID3D10Device::CreateTexture2D() on depthTexture failed.");
    }

    if (FAILED(hr = pd3dDevice->CreateDepthStencilView(depthTex.getData()->m_pTexture2D, &depthStencilViewDesc, &pDepthStencilView))) {
        return postException(resolveError(hr), "ID3D10Device::CreateDepthStencilView() on depthTexture failed.");
    }

    if (FAILED(hr = pd3dDevice->CreateShaderResourceView(depthTex.getData()->m_pTexture2D, &depthTexSRDesc, &depthTex.getData()->m_pShaderResource))) {
        return EventManager::Instance().postException(resolveError(hr), "ID3D10Device::CreateShaderResourceView() on depthTexture failed.");
    }
}

HRESULT DX11RenderSystem::createReflectBuffer()
{
    HRESULT hr;

    SceneManager.createTexture(reflectTex, "reflectionTexture", UVector2(0, 0), TEXTURE_NONE);

    D3D11_TEXTURE2D_DESC reflectTextureDesc;
    reflectTextureDesc.Width = r_ReflectMap.toVector().x;
    reflectTextureDesc.Height = r_ReflectMap.toVector().y;
    reflectTextureDesc.MipLevels = 1;
    reflectTextureDesc.ArraySize = 1;
    reflectTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    reflectTextureDesc.SampleDesc.Count = 1;
    reflectTextureDesc.SampleDesc.Quality = 0;

    reflectTextureDesc.Usage = D3D10_USAGE_DEFAULT;
    reflectTextureDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
    reflectTextureDesc.CPUAccessFlags = 0;
    reflectTextureDesc.MiscFlags = 0;

    D3D11_SHADER_RESOURCE_VIEW_DESC reflectSRDesc;

    reflectSRDesc.Format = reflectTextureDesc.Format;
    reflectSRDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    reflectSRDesc.Texture2D.MipLevels = 1;
    reflectSRDesc.Texture2D.MostDetailedMip = 0;

    if (FAILED(hr = pd3dDevice->CreateTexture2D(&reflectTextureDesc, NULL, &reflectTex.getData()->m_pTexture2D))) {
        return EventManager::Instance().postException(resolveError(hr), "ID3D10Device::CreateTexture2D() ReflectionTexture");
    }

    if (FAILED(hr = pd3dDevice->CreateRenderTargetView(reflectTex.getData()->m_pTexture2D, NULL, &pReflectionTargetView))) {
        return EventManager::Instance().postException(resolveError(hr), "ID3D10Device::CreateRenderTargetView() ReflectionTexture");
    }

    if (FAILED(hr = pd3dDevice->CreateShaderResourceView(reflectTex.getData()->m_pTexture2D, &reflectSRDesc, &reflectTex.getData()->m_pShaderResource))) {
        return EventManager::Instance().postException(resolveError(hr), "ID3D10Device::CreateShaderResourceView() ReflectionTexture");
    }

    return S_OK;
}

HRESULT DX11RenderSystem::createPreProcessBuffer()
{
    HRESULT hr;

    SceneManager.createTexture(preProcessTex, "preProcess", UVector2(0, 0), TEXTURE_NONE);

    D3D10_TEXTURE2D_DESC preprocessTextureDesc;
    preprocessTextureDesc.Width = r_WindowSize.toVector().x;
    preprocessTextureDesc.Height = r_WindowSize.toVector().y;
    preprocessTextureDesc.MipLevels = 1;
    preprocessTextureDesc.ArraySize = 1;
    preprocessTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    preprocessTextureDesc.SampleDesc.Count = 1;
    preprocessTextureDesc.SampleDesc.Quality = 0;

    preprocessTextureDesc.Usage = D3D10_USAGE_DEFAULT;
    preprocessTextureDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
    preprocessTextureDesc.CPUAccessFlags = 0;
    preprocessTextureDesc.MiscFlags = 0;

    D3D10_SHADER_RESOURCE_VIEW_DESC preprocessSRDesc;

    preprocessSRDesc.Format = preprocessTextureDesc.Format;
    preprocessSRDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    preprocessSRDesc.Texture2D.MipLevels = 1;
    preprocessSRDesc.Texture2D.MostDetailedMip = 0;

    if (FAILED(hr = pd3dDevice->CreateTexture2D(&preprocessTextureDesc, NULL, &preProcessTex.getData()->m_pTexture2D))) {
        return EventManager::Instance().postException(hr, "ID3D10Device::CreateTexture2D() PreProcessTexture");
    }

    if (FAILED(hr = pd3dDevice->CreateRenderTargetView(preProcessTex.getData()->m_pTexture2D, NULL, &pPreProcessTargetView))) {
        return EventManager::Instance().postException(hr, "ID3D10Device::CreateRenderTargetView() PreProcessTexture");
    }

    if (FAILED(hr = pd3dDevice->CreateShaderResourceView(preProcessTex.getData()->m_pTexture2D, &preprocessSRDesc, &preProcessTex.getData()->m_pShaderResource))) {
        return EventManager::Instance().postException(hr, "ID3D10Device::CreateShaderResourceView() PreProcessTexture");
    }

    return S_OK;
}

HRESULT DX11RenderSystem::createShadowMap()
{
    HRESULT hr;

    SceneManager.createTexture(shadowMapTex, "shadowMap", UVector2(0, 0), TEXTURE_NONE);

    // create the depthstencilview and the shaderresource for the shadowmap
    D3D11_TEXTURE2D_DESC shadowTextureDesc;
    shadowTextureDesc.Width = r_ShadowSize.toVector().x;
    shadowTextureDesc.Height = r_ShadowSize.toVector().y;
    shadowTextureDesc.MipLevels = 1;
    shadowTextureDesc.ArraySize = 1;
    shadowTextureDesc.Format = DXGI_FORMAT_R16_TYPELESS;

    shadowTextureDesc.SampleDesc.Count = 1;
    shadowTextureDesc.SampleDesc.Quality = 0;

    shadowTextureDesc.Usage = D3D10_USAGE_DEFAULT;
    shadowTextureDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
    shadowTextureDesc.CPUAccessFlags = 0;
    shadowTextureDesc.MiscFlags = 0;

    D3D11_SHADER_RESOURCE_VIEW_DESC shadowTextureSRDesc;

    shadowTextureSRDesc.Format = DXGI_FORMAT_R16_UNORM;
    shadowTextureSRDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    shadowTextureSRDesc.Texture2D.MipLevels = 1;
    shadowTextureSRDesc.Texture2D.MostDetailedMip = 0;

    D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
    DepthStencilViewDesc.Format = DXGI_FORMAT_D16_UNORM;
    DepthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    DepthStencilViewDesc.Texture2D.MipSlice = 0;

    if (FAILED(hr = pd3dDevice->CreateTexture2D(&shadowTextureDesc, NULL, &shadowMapTex.getData()->m_pTexture2D))) {
        return EventManager::Instance().postException(hr, "ID3D10Device::CreateDepthStencil() ShadowMapTexture");
    }

    if (FAILED(hr = pd3dDevice->CreateDepthStencilView(shadowMapTex.getData()->m_pTexture2D, &DepthStencilViewDesc, &pShadowMapView))) {
        return EventManager::Instance().postException(hr, "ID3D10Device::CreateDepthStencilView() ShadowMapTexture");
    }

    if (FAILED(hr = pd3dDevice->CreateShaderResourceView(shadowMapTex.getData()->m_pTexture2D, &shadowTextureSRDesc, &shadowMapTex.getData()->m_pShaderResource))) {
        return EventManager::Instance().postException(hr, "ID3D10Device::CreateShaderResourceView() ShadowMapTexture");
    }

    return S_OK;
}

HRESULT DX11RenderSystem::createBlendStates()
{
    ::D3D10_BLEND_DESC blendDesc;

    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.BlendEnable[0] = true;
    blendDesc.BlendOp = D3D10_BLEND_OP_ADD;
    blendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
    blendDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
    blendDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
    blendDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTargetWriteMask[1] = D3D10_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTargetWriteMask[2] = D3D10_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTargetWriteMask[3] = D3D10_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTargetWriteMask[4] = D3D10_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTargetWriteMask[5] = D3D10_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTargetWriteMask[6] = D3D10_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTargetWriteMask[7] = D3D10_COLOR_WRITE_ENABLE_ALL;
    blendDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
    blendDesc.SrcBlendAlpha = D3D10_BLEND_ZERO;

    HRESULT hr = S_OK;

    if (FAILED(hr = pd3dDevice->CreateBlendState(&blendDesc, &pBlendEnabled))) {
        return EventManager::Instance().postException(hr, "CreateBlendState() failed");
    }

    blendDesc.BlendEnable[0] = false;

    if (FAILED(hr = pd3dDevice->CreateBlendState(&blendDesc, &pBlendDisabled))) {
        return EventManager::Instance().postException(hr, "CreateBlendState() failed");
    }

    setBlendState(false);

    return S_OK;
}

int DX11RenderSystem::createDepthStates()
{
    // the depth stencil state
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = D3D10_DEFAULT_STENCIL_READ_MASK;
    depthStencilDesc.StencilWriteMask = D3D10_DEFAULT_STENCIL_WRITE_MASK;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    HRESULT hr = S_OK;

    if (FAILED(hr = pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_pEnabledDepthState))) {
        return postException(resolveError(hr), "Creating the enabled depth state failed.");
    }

    depthStencilDesc.DepthEnable = false;
    depthStencilDesc.StencilEnable = false;

    if (FAILED(hr = pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDisabledDepthState))) {
        return postException(resolveError(hr), "Creating the disabled depth state failed.");
    }

    setDepthState(true);

    return 0;
}

void DX11RenderSystem::setViewports()
{
    // Set viewports
    viewPort[0].Width = r_WindowSize.toVector().x;
    viewPort[0].Height = r_WindowSize.toVector().y;
    viewPort[0].MinDepth = 0.0f;
    viewPort[0].MaxDepth = 1.0f;
    viewPort[0].TopLeftX = 0;
    viewPort[0].TopLeftY = 0;

    viewPort[1].Width = r_ShadowSize.toVector().x;
    viewPort[1].Height = r_ShadowSize.toVector().y;
    viewPort[1].MinDepth = 0.0f;
    viewPort[1].MaxDepth = 1.0f;
    viewPort[1].TopLeftX = 0;
    viewPort[1].TopLeftY = 0;

    viewPort[2].Width = (UINT)r_WindowSize.toVector().x;
    viewPort[2].Height = (UINT)r_WindowSize.toVector().y;
    viewPort[2].MinDepth = 0.0f;
    viewPort[2].MaxDepth = 1.0f;
    viewPort[2].TopLeftX = 0;
    viewPort[2].TopLeftY = 0;

    Surface.resize(viewPort[0]);
}

void DX11RenderSystem::setRasterizerState(bool bWireframe)
{
    // set rasterizer	
    pd3dDevice->RSSetState(NULL);

    D3D10_RASTERIZER_DESC rasterizerDescription;
    rasterizerDescription.CullMode = D3D10_CULL_BACK;
    rasterizerDescription.FillMode = D3D10_FILL_SOLID;
    if (bWireframe)
    {
        rasterizerDescription.FillMode = D3D10_FILL_WIREFRAME;
        rasterizerDescription.CullMode = D3D10_CULL_NONE;
    }
    rasterizerDescription.FrontCounterClockwise = false;
    rasterizerDescription.DepthBias = false;
    rasterizerDescription.DepthBiasClamp = 0;
    rasterizerDescription.SlopeScaledDepthBias = 0;
    rasterizerDescription.DepthClipEnable = true;
    rasterizerDescription.ScissorEnable = false;
    rasterizerDescription.MultisampleEnable = (r_MultiSamples.toInt() > 0);
    rasterizerDescription.AntialiasedLineEnable = true;

    ID3D10RasterizerState* pRS;

    pd3dDevice->CreateRasterizerState(&rasterizerDescription, &pRS);
    pd3dDevice->RSSetState(pRS);
}

void DX11RenderSystem::setSwapchain()
{
    try
    {
        DXGI_MODE_DESC Desc;
        Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        Desc.Height = r_WindowSize.toVector().x;
        Desc.Width = r_WindowSize.toVector().y;
        Desc.RefreshRate.Numerator = r_RefreshRate.toInt();
        Desc.RefreshRate.Denominator = 1;
        Desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        Desc.Scaling = DXGI_MODE_SCALING_STRETCHED;

        DXGI_MODE_DESC OptimalDesc;

        IDXGIOutput* pAdapterOutput;

        pSwapChain->GetContainingOutput(&pAdapterOutput);

        HRESULT hr;

        if (FAILED(hr = pAdapterOutput->FindClosestMatchingMode(&Desc, &OptimalDesc, pd3dDevice)))
            throw(hr);

        if (FAILED(hr = pSwapChain->ResizeTarget(&OptimalDesc)))
            throw(hr);

        if (FAILED(hr = pSwapChain->SetFullscreenState(r_Fullscreen.toBool(), NULL)))
            throw(hr);
    }
    catch (HRESULT hr)
    {
        EventManager::Instance().postMessage("failed to change swapchain, with error %d", (int)hr);
    }
}