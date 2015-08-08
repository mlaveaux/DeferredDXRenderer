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

#ifndef DX11SURFACE_H
#define DX11SURFACE_H

#include "DX11Context.h"
#include "DX11Texture.h"

#include <SpriteBatch.h>

/**
 * The data of a single sprite.
 */
struct Sprite
{
    glm::vec2 texCoord; // Source texture texcoord.
    glm::vec2 texSize; // Source texture texture size.
    Texture texture; // The source texture reference.

    UVector2 pos; // The destination position.
    UVector2 size; // The destination size.
    UINT uLayer = 0; // The destination depth.

    glm::vec4 color; // Texture to apply to every pixel of the sprite.
};

class DX11SceneManager;

/**
 * The class that renders sprites.
 */
class DX11Surface
{
public:
    DX11Surface(DX11Context::Ptr context, DX11SceneManager& sceneManager);

    /**
     * Convenient function to load textures for sprites.
     */
    HRESULT loadTexture(Texture& textureRef, const char* strFilename);

    UINT getRenderCalls();

    void resize(D3D11_VIEWPORT viewPort);
    void drawSprite(Sprite& pSprite);
    void drawSpriteArray(Sprite* pSpriteArray, UINT uNumOfSprites);
    void Begin();
    void End();

private:
    DX11Context::Ptr m_context;
    DX11SceneManager& m_sceneManager;

    std::unique_ptr<DirectX::SpriteBatch> m_spriteBuffer;

    D3D11_VIEWPORT m_viewPort;
    UINT m_uRenderCalls;
};

#endif // DX11SURFACE_H