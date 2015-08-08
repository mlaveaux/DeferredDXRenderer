#include "Terrain.h"

#include <main/GlobalFunctions.h>
#include <main/EventManager.h>

#define LOWESTLOD 2
#define D3DX_PI 3.14f

Terrain::Terrain(DX10SceneManager* pSceneManager) : IEventListener("terrain")
{
    m_pSceneManager = pSceneManager;
}

Terrain::~Terrain()
{
    m_pSceneManager = NULL;

    SAFE_DELETE_ARRAY(m_pHeightArray);
    SAFE_DELETE_ARRAY(m_pChunkArray);
    SAFE_DELETE_ARRAY(m_pPathMap);
}

HRESULT Terrain::create(UVector2 terrainSize)
{
    //The size of the biggest lod.
    m_chunkSize = (UINT)::pow(2.0f, LOWESTLOD) * 32;
    //Number of chunks required (only multiples of 32 currently)
    m_numberOfChunks.x = (int)(terrainSize.x / m_chunkSize);
    m_numberOfChunks.y = (int)(terrainSize.y / m_chunkSize);

    //Number of vertices
    m_vertexSize.x = terrainSize.x + 1;
    m_vertexSize.y = terrainSize.y + 1;

    //array lenght
    UINT uLenght = m_vertexSize.x * m_vertexSize.y;

    //create a new heightmap
    m_pHeightArray = new float[uLenght];

    //Fill it with zeroes
    UINT i = 0;
    for (i = 0; i < uLenght; i++)
    {
        m_pHeightArray[i] = 0.0f;
    }

    //Normal map.
    m_pSceneManager->createTexture(normalTexture, "normalTex", m_vertexSize, TEXTURE_RGBA8);
    //Per chunk 4byte maskTexture.
    m_pSceneManager->createTexture(m_maskTexture, "maskTex", m_vertexSize, TEXTURE_RGBA8);

    UINT x = 0;
    UINT z = 0;
    i = 0;

    //Fill or load the maskTexture
    BYTE* pTexelArray = m_pSceneManager->map(m_maskTexture);

    for (UINT v = 0; v < m_vertexSize.y; v++)
    {
        for (UINT u = 0; u < m_vertexSize.x; u++)
        {
            //RGBA of 4 byte size.
            pTexelArray[v * m_maskTexture.getData()->m_rowPitch + u * 4 + 0] = 255;
            pTexelArray[v * m_maskTexture.getData()->m_rowPitch + u * 4 + 1] = 0;
            pTexelArray[v * m_maskTexture.getData()->m_rowPitch + u * 4 + 2] = 0;
            pTexelArray[v * m_maskTexture.getData()->m_rowPitch + u * 4 + 3] = 0;
        }
    }

    m_pSceneManager->unmap(m_maskTexture);

    //Create the chunks
    m_pChunkArray = new TerrainChunk[m_numberOfChunks.x * m_numberOfChunks.y];

    i = 0;
    x = 0;
    z = 0;
    UVector2 offset;

    //Create all the chunks
    for (z = 0; z < m_numberOfChunks.y; z++)
    {
        for (x = 0; x < m_numberOfChunks.x; x++)
        {
            //Initialize the chunk ready for usage.
            if (FAILED(m_pChunkArray[i].initialize(m_pSceneManager)))
                return E_FAIL;
            //Calculate the offset of the chunk (from topleft)
            offset = UVector2(x*m_chunkSize, (m_numberOfChunks.y - z)*m_chunkSize + 1);
            //Create the chunk mesh and starts displaying it.
            if (FAILED(m_pChunkArray[i].create(m_pHeightArray, m_maskTexture, m_vertexSize, offset, LOWESTLOD)))
                return E_FAIL;

            i++;
        }
    }

    //Post the current terrain specifications
    Event terrainLoaded("terrain", 0);
    //postEvent(EVT_TERRAIN_NAME, 0 , (char*)"custom_terrain");
    postEvent(terrainLoaded);

    m_water.initialize(m_pSceneManager);

    return S_OK;
};

HRESULT Terrain::load(const char* strFilename)
{

    return S_OK;
}

HRESULT Terrain::save(const char* strFilename)
{
    UINT Offset = 0;
    DWORD dwArraySize = m_vertexSize.x * m_vertexSize.y + 2 * 2;
    BYTE* pbBuffer = (BYTE*)malloc(dwArraySize);

    writeWord(&Offset, (WORD)m_vertexSize.x, pbBuffer);
    writeWord(&Offset, (WORD)m_vertexSize.y, pbBuffer);

    for (UINT i = 0; i < (m_vertexSize.x * m_vertexSize.y); i++)
    {
        writeWord(&Offset, (WORD)(m_pHeightArray[i] * 10), pbBuffer);
    }

    HANDLE hFile = NULL;
    DWORD dwBytesWritten = 0;

    try
    {
        hFile = ::CreateFile(strFilename,
            FILE_WRITE_DATA,
            FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (hFile == INVALID_HANDLE_VALUE)
            throw(GetLastError());

        //Write the file
        if (!::WriteFile(hFile, pbBuffer, dwArraySize, &dwBytesWritten, NULL))
        {
            throw(GetLastError());
        }

        if (dwArraySize != dwBytesWritten)
        {
            throw(GetLastError());
        }

        ::CloseHandle(hFile);
        SAFE_DELETE_ARRAY(pbBuffer);
    }
    catch (DWORD Error)
    {
        ::CloseHandle(hFile);
        SAFE_DELETE_ARRAY(pbBuffer);

        return Error;
    }

    return S_OK;
}

bool Terrain::intersect(glm::vec3 RayOrigin, glm::vec3 RayDirection, glm::vec3& pHitPos)
{
    UINT uHitCount = 0;
    UINT whichFace = 0;

    //float barycentricHitU = 0;
    //float barycentricHitV = 0;

    UINT Steps = abs((int)(RayOrigin.y / RayDirection.y)) + 1;
    float distance = 0;
    float Height = 0;

    for (UINT i = 0; i < Steps; i++)
    {
        RayOrigin += RayDirection;
        if (RayOrigin.x > 0.0f && RayOrigin.x < m_vertexSize.x && RayOrigin.z > 0.0f && RayOrigin.z < m_vertexSize.y)
        {
            Height = getMinHeight(glm::vec2(RayOrigin.x, RayOrigin.z));
            if (RayOrigin.y <= Height)
            {
                pHitPos.x = RayOrigin.x;
                pHitPos.y = RayOrigin.y;
                pHitPos.z = RayOrigin.z;
                return true;
            }
        }
    }
    return false;
}

float Terrain::getMinHeight(glm::vec2 point)
{
    //get the closest heightmap point
    int posX = (int)point.x;
    int posY = (int)point.y;

    //if it is outside of the heightmap give mapheight 0.0f 
    //(might change to closest point available on heightmap)
    if (point.x < 0.0f || point.x > m_vertexSize.x || point.y < 0.0f || point.y > m_vertexSize.y)
        return 0.0f;

    //calculate the index
    int i = posX + (m_vertexSize.y - posY) * m_vertexSize.x;
    float Height = m_pHeightArray[i];

    float heightDeltaX = 0;
    float heightDeltaY = 0;

    //point relative to the closest vertex
    glm::vec2 relativePos;
    relativePos.x = point.x - posX;
    relativePos.y = point.y - posY;

    //y > -1*x for (x,y)
    //implies y < x (mathematics)
    if (relativePos.y < relativePos.x)
    {
        heightDeltaX = m_pHeightArray[i + 1] - Height;
        heightDeltaY = m_pHeightArray[i + m_vertexSize.x + 1] - m_pHeightArray[i + 1];
    }

    //y < -1*x for (x,y)
    //implies y > x (mathematics)
    if (relativePos.y > relativePos.x)
    {

        heightDeltaX = m_pHeightArray[i + m_vertexSize.x + 1] - m_pHeightArray[i + m_vertexSize.x];
        heightDeltaY = m_pHeightArray[i + m_vertexSize.x] - Height;
    }

    //return Height + linear interpolation x + linear interpolation y;
    return Height + relativePos.x * heightDeltaX + relativePos.y * heightDeltaY;
}

void Terrain::editTerrain(glm::vec2 Pos, float fTime, float fStrenght, float fBrushSize, int iType)
{
    //Radius and brushsizein UINT
    UINT uRadius = (UINT)(0.5f * fBrushSize + 1.0f);
    UINT uBrushSize = (UINT)(fBrushSize + 1.0f);
    //The topleft x,z from the part that will be editted.
    int x_min = (int)(Pos.x - uRadius);
    int z_min = (int)(Pos.y + uRadius);
    //x cant be lower then 0 and z cant be greater then m_vertexSize
    if (x_min < 0) {
        x_min = 0;
    }
    if (z_min > (int)m_vertexSize.y) {
        z_min = (int)m_vertexSize.y;
    }

    UINT i = 0;
    UINT x = 0;
    UINT z = 0;

    //Check which chunks are affected too set their update flag
    for (i = 0; i < m_numberOfChunks.x * m_numberOfChunks.y; i++)
    {
        m_pChunkArray[i].updateRequired(UVector2(x_min, z_min), uBrushSize);
    }

    UINT current = 0;
    int Index = 0;
    i = 0;
    //Loop through those chunkds and edit them
    for (z = z_min; z > (z_min - uBrushSize); z--) {
        for (x = x_min; x < (x_min + uBrushSize); x++) {
            glm::vec2 TempVector = glm::vec2((float)x, (float)z);

            float fDistance = glm::length(TempVector - Pos);
            //Distance to the center of the brush.
            if (fDistance <= uRadius) {
                i = x + (m_vertexSize.y - z) * m_vertexSize.x;
                float fHeight = 0;
                //Edittype
                switch (iType) {
                case 0:
                    fHeight = fStrenght * (1 + cosf((fDistance*(float)D3DX_PI) / uRadius));
                    break;

                case 1:
                    fHeight = -1.0f * fStrenght * (1 + cosf((fDistance*(float)D3DX_PI) / uRadius));
                    break;

                case 2:
                    fHeight = fStrenght - 1.0f * powf((1 / uRadius) * fDistance, fBrushSize);
                    break;

                case 3:
                    fHeight = -1.0f * fStrenght + powf((1 / uRadius) * fDistance, fBrushSize);

                case 4:
                    if (m_pHeightArray[i] > fStrenght) {
                        fHeight = fStrenght - m_pHeightArray[i] + -1.0f * powf((1 / uRadius) * fDistance, fBrushSize);
                    }
                    if (m_pHeightArray[i] < fStrenght) {
                        fHeight = fStrenght + m_pHeightArray[i] + -1.0f * powf((1 / uRadius) * fDistance, fBrushSize);
                    }
                    break;
                }

                m_pHeightArray[i] += fHeight * fTime * 0.001f;
            }
        }
    }
}

void Terrain::editTexture(glm::vec2 Pos, float fTime, float fStrenght, float fBrushSize, int iType)
{
    //Radius and brushsizein UINT
    UINT uRadius = (UINT)(0.5f * fBrushSize + 1.0f);
    UINT uBrushSize = (UINT)(fBrushSize + 1.0f);

    //The topleft x,z from the part that will be editted.
    int x_min = (int)(Pos.x - uRadius);
    int z_min = (int)(Pos.y + uRadius);

    //x cant be lower then 0 and z cant be greater then m_vertexSize
    if (x_min < 0) {
        x_min = 0;
    }
    if (z_min > (int)m_vertexSize.y) {
        z_min = (int)m_vertexSize.y;
    }

    //Load the maskTexture
    BYTE* pTexelArray = m_pSceneManager->map(m_maskTexture);

    UINT x;
    UINT z;
    UINT index;

    for (z = z_min; z > (z_min - uBrushSize); z--) {
        for (x = x_min; x < (x_min + uBrushSize); x++) {
            glm::vec2 TempVector = glm::vec2((float)x, (float)z);

            float fDistance = glm::length(TempVector - Pos);
            if (fDistance < uRadius) {
                index = z * m_maskTexture.getData()->m_rowPitch + x * 4;
                //if(pTexelArray[index] > 0)
                pTexelArray[index] = 0;
                //if(pTexelArray[index+1] < 255)
                pTexelArray[index + 1] = 0;
                pTexelArray[index + 2] = 0;
                pTexelArray[index + 3] = 0;
                switch (iType) {
                case 0:
                    pTexelArray[index] = 255;
                    break;

                case 1:
                    pTexelArray[index + 1] = 255;
                    break;
                }
            }
        }
    }

    m_pSceneManager->unmap(m_maskTexture);
}

void Terrain::updateLod(glm::vec3 cameraPos)
{
    for (UINT i = 0; i < m_numberOfChunks.x * m_numberOfChunks.y; i++) {
        //Call update for every chunk
        m_pChunkArray[i].updateLod(cameraPos, true);
    }
}

void Terrain::update()
{
    for (UINT i = 0; i < m_numberOfChunks.x * m_numberOfChunks.y; i++) {
        //Call update for every chunk
        m_pChunkArray[i].update();
    }
}

void Terrain::handleEvent(Event& e)
{

}