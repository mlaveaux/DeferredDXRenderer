#include "DX11Surface.h"
#include "DX11SceneManager.h"

#include "main/EventManager.h"

#include <iterator>

DX11Surface::DX11Surface(DX11Context::Ptr context, DX11SceneManager& sceneManager)
    : m_context(context), m_sceneManager(sceneManager)
{
    HRESULT hr;
    m_spriteBuffer = std::unique_ptr<DirectX::SpriteBatch>(new DirectX::SpriteBatch(context->getContext()));
}

void DX11Surface::resize(D3D11_VIEWPORT viewPort)
{
    m_spriteBuffer->SetViewport(viewPort);
}

HRESULT DX11Surface::loadTexture(Texture& textureRef, const char* strTextureFilename)
{
    return m_sceneManager.loadTexture(textureRef, strTextureFilename);
}

UINT DX11Surface::getRenderCalls()
{
    return m_uRenderCalls;
}

void DX11Surface::drawSprite(Sprite& sprite)
{
    if (sprite.texture.getLoadState() != STATE_LOADED) {
        return; // Only draw when the texture was properly loaded.
    }

    DirectX::FXMVECTOR position = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(sprite.pos.x, sprite.pos.y, sprite.pos.z));
    DirectX::FXMVECTOR color = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a));

    m_spriteBuffer->Draw(
        sprite.texture.getData()->m_pShaderResource,
        position,
        color);

    m_uRenderCalls++;
}

void DX11Surface::drawSpriteArray(Sprite* pSpriteArray, UINT uNumOfSprites)
{
    assert(pSpriteArray != nullptr);

    for (int i = 0; i < uNumOfSprites; ++i) {
        drawSprite(pSpriteArray[i]);
    }

    m_uRenderCalls += uNumOfSprites;
}

void DX11Surface::Begin()
{
    m_uRenderCalls = 0;

    m_spriteBuffer->Begin();
}

void DX11Surface::End()
{
    m_spriteBuffer->End();
}