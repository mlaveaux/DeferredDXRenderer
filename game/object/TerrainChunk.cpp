#include "TerrainChunk.h"

TerrainChunk::TerrainChunk()
{
    m_bUpdateRequired = false;
}

TerrainChunk::~TerrainChunk()
{
    SAFE_DELETE_ARRAY(m_pVertexArray);
    SAFE_DELETE_ARRAY(m_pIndexArray);
}

HRESULT TerrainChunk::initialize(DX10SceneManager* pSceneManager)
{
    //Arrays to temporarily store the mesh
    m_pVertexArray = new Vertex[33 * 33];
    m_pIndexArray = new UINT[33 * 33 * 6];

    m_pSceneManager = pSceneManager;
    m_pShaderSystem = pSceneManager->getShaderSystem();

    dynamicModel = m_pSceneManager->createModel();

    return S_OK;
}

HRESULT TerrainChunk::create(float *pHeightArray, Texture maskTexture, UVector2 vertexSize, UVector2 Offset, UINT lodLevel)
{
    HRESULT hr;
    //Information of the Terrain.
    m_pHeightArray = pHeightArray;
    m_vertexSize = vertexSize;
    m_Offset = Offset;
    m_lodLevel = lodLevel;
    m_vertexOffset = (UINT)::pow(2.0f, (int)m_lodLevel);
    m_midPoint = UVector2(m_Offset.x + 16 * m_vertexOffset, m_Offset.y - (int)(16 * m_vertexOffset));

    //Copy a reference to the maskTexture
    m_maskTexture = maskTexture;
    //4 blendtextures for the texture mask.
    SceneManager()->loadTexture(texture0, "terrain/Grass.dds");
    SceneManager()->loadTexture(texture1, "terrain/Dirt.dds");
    SceneManager()->loadTexture(texture2, "terrain/Sand.dds");
    SceneManager()->loadTexture(texture3, "terrain/rock.dds");

    Texture shadowMap;
    SceneManager()->loadTexture(shadowMap, "shadowMap");

    //Fill the boundingBox.
    glm::vec3 boundingBox[4];
    boundingBox[0] = glm::vec3(0.0f, 0.0f, 0.0f);
    boundingBox[1] = glm::vec3(32.0f, 0.0f, 0.0f);
    boundingBox[2] = glm::vec3(0.0f, 0.0f, -32.0f);
    boundingBox[3] = glm::vec3(32.0f, 0.0f, -32.0f);
    dynamicModel->setBoundingBox(boundingBox);

    m_pChunkNode = SceneManager()->createSceneNode("terrain", SceneManager()->getMainNode());
    m_pChunkNode->attachModel(dynamicModel->getID());
    //m_pChunkNode->translate(Offset.x, 0.0f, Offset.y-1.0f);

    fillMesh();

    if (FAILED(hr = dynamicModel->createMesh(m_pVertexArray, 33 * 33, m_pIndexArray, 33 * 33 * 6)))
        return EventManager::Instance().postException(hr, "Initializing the terrainmesh failed");

    //Load the terrain shader	
    DX10Shader terrainShader = m_pShaderSystem->initShader("terrainShader.fx", VERTEX);
    //Set the blend and shadowmap textures
    terrainShader.bindTexture(maskTexture, "blendTex");
    terrainShader.bindTexture(shadowMap, "shadowMap");
    //Bind the 4 textures that blend
    Texture texArray[] = { texture0, texture1, texture2, texture3 };
    terrainShader.bindTextureArray(texArray, "textureArray", 4);
    //The scale of the uvcoordinates
    terrainShader.bindValue(CValue(8 * (int)(vertexSize.x / 32)), "uscale");
    terrainShader.bindValue(CValue(8 * (int)(vertexSize.y / 32)), "vscale");

    glm::mat4x4 m_world(1.0f);
    m_world = glm::translate(m_world, glm::vec3((float)m_Offset.x, 0.0f, (float)m_Offset.y));

    dynamicModel->setMatrix(m_world);
    dynamicModel->setShader(terrainShader);

    if (m_lodLevel > 0) {
        m_childChunks = new TerrainChunk[4];
        UINT i = 0;
        UINT x = 0;
        UINT z = 0;
        UINT m_childLod = m_lodLevel - 1;

        for (z = 0; z < 2; z++) {
            for (x = 0; x < 2; x++) {
                if (FAILED(m_childChunks[i].initialize(m_pSceneManager))) {
                    return E_FAIL;
                }

                UVector2 offsetVector = UVector2(m_Offset.x + x * 16 * m_vertexOffset, m_Offset.y - (int)z * 16 * m_vertexOffset);
                if (FAILED(m_childChunks[i].create(m_pHeightArray, m_maskTexture, vertexSize, offsetVector, m_childLod))) {
                    return E_FAIL;
                }

                i++;
            }
        }

        m_pChunkNode->setVisible(false);
    }

    return S_OK;
}

void TerrainChunk::updateRequired(UVector2 leftTop, UINT brushSize)
{
    if (m_lodLevel > 0) {
        for (int i = 0; i < 4; i++) {
            m_childChunks[i].updateRequired(leftTop, brushSize);
        }
    }

    if (m_bUpdateRequired) {
        return;
    }

    UINT offset = (UINT)::pow(2.0f, (int)m_lodLevel);
    UVector2 size(offset * 32, offset * 32);

    if (checkBoundaries(UVector2(leftTop.x, leftTop.y), m_Offset, size) ||
        checkBoundaries(UVector2(leftTop.x, leftTop.y - brushSize), m_Offset, size) ||
        checkBoundaries(UVector2(leftTop.x + brushSize, leftTop.y), m_Offset, size) ||
        checkBoundaries(UVector2(leftTop.x + brushSize, leftTop.y - brushSize), m_Offset, size))
        m_bUpdateRequired = true;
}

void TerrainChunk::updateLod(const glm::vec3& cameraPos, bool isVisible)
{
    //Copy the value of isVisible
    m_pChunkNode->setVisible(isVisible);

    glm::vec3 vector = glm::vec3((float)m_midPoint.x, 0.0f, (float)m_midPoint.y) - cameraPos;

    float distance = glm::length(vector);

    bool visible = false;

    if (m_lodLevel > 0) {
        if (isVisible) {
            if (distance < (m_lodLevel + 1)*(m_lodLevel + 1) * 10.0f) {
                m_pChunkNode->setVisible(false);
                visible = true;
            }
        }
        for (int i = 0; i < 4; i++) {
            m_childChunks[i].updateLod(cameraPos, visible);
        }
    }
}

void TerrainChunk::update()
{
    if (m_lodLevel > 0) {
        for (int i = 0; i < 4; i++) {
            m_childChunks[i].update();
        }
    }

    if (!m_bUpdateRequired || !m_pChunkNode->isVisible()) {
        return;
    }

    fillMesh();

    SceneManager()->UpdateVertex(dynamicModel->getMesh(), m_pVertexArray, 0, 33 * 33);
    SceneManager()->UpdateIndex(dynamicModel->getMesh(), m_pIndexArray, 0, 33 * 33 * 6);

    m_bUpdateRequired = false;
}

void TerrainChunk::fillMesh()
{
    //index of the heightmap
    UINT index = 0;
    //Index of the vertexArray
    UINT i = 0;
    //x and z of the position of the vertex
    UINT x = 0;
    UINT z = 0;
    //Texture UV coords
    float TexX = 0.0f;
    float TexZ = 0.0f;
    //Absolute size.
    UINT size = m_vertexOffset * 32;

    //Texture UV Size per vertex.
    float UMapSize = (1.0f / (float)m_vertexSize.x);
    float VMapSize = (1.0f / (float)m_vertexSize.y);
    //Rows
    for (z = 0; z < size + 1; z += m_vertexOffset) {
        //Columns
        TexZ = (m_Offset.y - z) * VMapSize;
        for (x = 0; x < size + 1; x += m_vertexOffset) {
            TexX = (x + m_Offset.x) * UMapSize;

            index = m_Offset.x + x + (m_vertexSize.y - m_Offset.y + z) * m_vertexSize.x;

            m_pVertexArray[i].Pos = glm::vec3((float)x, m_pHeightArray[index], -1.0f * (float)z);
            m_pVertexArray[i].TexCoord = glm::vec2(TexX, TexZ);
            m_pVertexArray[i].Normal = calculateNormal(m_Offset.x + x, m_vertexSize.y - m_Offset.y + z, index);

            if (z < size && x < size) {
                updateIndexData(i, index, m_pIndexArray);
            }

            i++;
        }
    }
}

void TerrainChunk::updateIndexData(int i, int index, UINT* pIndexArray)
{
    float Delta = m_pHeightArray[index] - m_pHeightArray[index + m_vertexSize.x + 1];
    //if(Delta < 0.0f)
    //	Delta*=-1.0f;
    float Delta2 = m_pHeightArray[index + 1] - m_pHeightArray[index + m_vertexSize.x];
    //if(Delta2 < 0.0f)
    //	Delta2*=-1.0f;

    int current = i * 6;

    if (Delta >= Delta2) {
        //bottomleft to topright polygonal layout
        pIndexArray[current] = i + 1;
        pIndexArray[current + 1] = i + 33 + 1;
        pIndexArray[current + 2] = i + 33;
        pIndexArray[current + 3] = i + 33;
        pIndexArray[current + 4] = i;
        pIndexArray[current + 5] = i + 1;
    }
    else {
        //bottomright to topleft polygonal layout
        pIndexArray[current] = i;
        pIndexArray[current + 1] = i + 1;
        pIndexArray[current + 2] = i + 33 + 1;
        pIndexArray[current + 3] = i + 33 + 1;
        pIndexArray[current + 4] = i + 33;
        pIndexArray[current + 5] = i;
    }
}

glm::vec3 TerrainChunk::calculateNormal(int x, int z, int i)
{
    glm::vec3 leftVector;
    if (x == 0) {
        leftVector = glm::vec3(-1.0f, 0.0f, 0.0f);
    }
    else {
        leftVector = glm::vec3(-1.0f, m_pHeightArray[i - 1] - m_pHeightArray[i], 0.0f);
    }

    glm::vec3 upVector;
    if (z == 0) {
        upVector = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    else {
        upVector = glm::vec3(0.0f, m_pHeightArray[i - m_vertexSize.x] - m_pHeightArray[i], 1.0f);
    }

    glm::vec3 rightVector;
    if (x == (m_vertexSize.x - 1)) {
        rightVector = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    else {
        rightVector = glm::vec3(1.0f, m_pHeightArray[i + 1] - m_pHeightArray[i], 0.0f);
    }

    glm::vec3 downVector;
    if (z == (m_vertexSize.y - 1)) {
        downVector = glm::vec3(0.0f, 0.0f, -1.0f);
    }
    else {
        downVector = glm::vec3(0.0f, m_pHeightArray[i + m_vertexSize.x] - m_pHeightArray[i], -1.0f);
    }
    
    upVector = glm::normalize(upVector);
    downVector = glm::normalize(downVector);
    leftVector = glm::normalize(leftVector);
    rightVector = glm::normalize(rightVector);

    glm::vec3 normal1;
    glm::vec3 normal2;

    normal1 = glm::cross(leftVector, upVector);
    normal2 = glm::cross(rightVector, downVector);

    normal1 = glm::normalize(normal1);
    normal2 = glm::normalize(normal2);

    glm::vec3 normal = (normal1 + normal2) * 0.5f;

    normal = glm::normalize(normal);

    return normal;
}