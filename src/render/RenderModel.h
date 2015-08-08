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

#ifndef RENDERMODEL_H
#define RENDERMODEL_H

#include "render/DX11/DX11SceneManager.h"
#include "render/DX11/DX11ShaderSystem.h"

#include <array>

struct DX11Mesh
{
    DX11Mesh()
    {
        pVertexBuffer = NULL;
        pIndexBuffer = NULL;
    }

    ~DX11Mesh()
    {
        uNumOfVertices = 0;
        uNumOfIndices = 0;
        SAFE_RELEASE(pVertexBuffer);
        SAFE_RELEASE(pIndexBuffer);
        pVertexBuffer = NULL;
        pIndexBuffer = NULL;
    }

    D3D11_PRIMITIVE_TOPOLOGY Topology;

    ID3D11Buffer* pVertexBuffer;
    UINT uNumOfVertices;

    ID3D11Buffer* pIndexBuffer;
    UINT uNumOfIndices;
};

/**
 * A 3Dmodel consists of a 3D Mesh, a Shader and a boundingbox.
 */
class RenderModel
{
public:
    //The constructors that is called on createModel().
    RenderModel(UINT index, DX11SceneManager* pSceneManager, DX11ShaderSystem* pShaderSystem) : m_world(1.0f)
    {
        m_indexId = index;
        m_pSceneManager = pSceneManager;
        m_pShaderSystem = pShaderSystem;
    }

    //Create the mesh from ordinary data.
    HRESULT createMesh(Vertex* pVertexArray, UINT uNumOfVertices, UINT* pIndexBuffer, UINT uNumOfIndices)
    {
        return m_pSceneManager->createMesh(&m_mesh, pVertexArray, uNumOfVertices, pIndexBuffer, uNumOfIndices);
    }

    //Change the boundingbox.
    void setBoundingBox(glm::vec3 box[4])
    {
        m_boundingBox[0] = box[0];
        m_boundingBox[1] = box[1];
        m_boundingBox[2] = box[2];
        m_boundingBox[3] = box[3];
    }

    //Set the world matrix of the model.
    void setMatrix(glm::mat4x4 matrix) { m_world = matrix; }

    //Set the shader to render the model.
    void setShader(DX11Shader shader) { m_shader = shader; }

    glm::mat4x4  getMatrix() { return m_world; }
    DX11Mesh&   getMesh()   { return m_mesh; }
    DX11Shader& getShader() { return m_shader; }
    UINT        getID()     { return m_indexId; }
    std::array<glm::vec3, 4>& getBoundingBox() { return m_boundingBox; }

protected:
    DX11SceneManager* m_pSceneManager;
    DX11ShaderSystem* m_pShaderSystem;
    DX11Mesh m_mesh;
    DX11Shader m_shader;

    std::array<glm::vec3, 4> m_boundingBox;
    glm::mat4x4 m_world;
    UINT m_indexId;

    UINT m_lowerLod;
};

#endif // RENDERMODEL_H