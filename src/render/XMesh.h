#ifndef XMESH_H
#define XMESH_H

#include "render/RenderModel.h"
#include "render/SceneNode.h"
#include "main/GlobalFunctions.h"

#include <glm/glm.hpp>

#include <windows.h>
#include <fstream>
#include <string.h>

enum CoordinateSystem
{
    LEFTHANDED = 0,
    RIGHTHANDED = 1,
};

struct Material
{
    std::string strName;

    Texture diffuseTex;
    Texture specularTex;
    Texture normalTex;
};

#define CHILD_FRAME_FOUND 71273

class Frame
{
public:
    Frame(DX11SceneManager* pSceneManager, DX11ShaderSystem* pShaderSystem)
    {
        m_pSceneManager = pSceneManager;
        m_pShaderSystem = pShaderSystem;
    }

    ~Frame()
    {
        SAFE_DELETE(m_pModel);
        SAFE_DELETE(m_pSceneNode);
    }

    void setMaterial(Material material)
    {
        m_material = material;
        m_pModel->getShader().bindTexture(m_material.diffuseTex, "diffuseTex");
    }

    void updateMatrixName(const char* strName, glm::mat4x4 Matrix)
    {
        if (m_strName.compare(strName) == 0) {
            m_pModel->setMatrix(Matrix);
            return;
        }
    }

    // returns the SceneNode of this frame
    SceneNode*   getNode() { return m_pSceneNode; }
    // returns the model of this frame
    RenderModel* getModel() { return m_pModel; }
    // returns the name of the frame
    const char*  getName() { return m_strName.c_str(); }

    //Creates a new frame, only used by XMesh class
    int initialize(SceneNode* pSceneNode, glm::mat4x4 world, std::string strName, Vertex* pVertexArray, UINT vertexCount, UINT* pIndexArray, UINT indexCount);

private:
    DX11SceneManager* m_pSceneManager;
    DX11ShaderSystem* m_pShaderSystem;

    std::string    m_strName;
    RenderModel*   m_pModel;
    SceneNode*     m_pSceneNode;

    Material   m_material;
};

/**
 * Loading multiple object into one XMesh
 * Loading text .x files.
 * Hard edges for faces
 *
 * TODO:
 * Loading binary .x files.
 */
class XMesh
{
public:
    XMesh(DX11SceneManager* pSceneManager);

    /**
     *
     */
    HRESULT load(const char* Filename, CoordinateSystem type);

    //Change a matrix of a specific child ( use getChild() SceneNode)
    void setMatrixByName(char* Name, glm::mat4x4 Matrix);
    Frame* getFrameByName(const char* strName);
private:
    HRESULT readFrame(SceneNode* pParent, std::string currentLine, std::ifstream& inFile);
    HRESULT readMaterialText(std::string CurrentLine, std::ifstream& inFile, Material& material);

    DX11SceneManager* m_pSceneManager;
    DX11ShaderSystem* m_pShaderSystem;

    CoordinateSystem m_type;

    std::vector<Frame> m_frameContainer;
};

#endif // XMESH_H