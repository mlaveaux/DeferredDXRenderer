#ifndef TERRAINCHUNK_H
#define TERRAINCHUNK_H

#include <array>
#include <object/Entity.h>
#include <render/RenderModel.h>
#include <render/SceneNode.h>

class TerrainChunk 
{
public:
	TerrainChunk();
	~TerrainChunk();

	//Makes the TerrainChunk ready to use.
	HRESULT initialize(DX10SceneManager* pSceneManager);

	//Create a new chunk for a certain area.
	HRESULT create(float* pHeightArray, Texture maskTexture, UVector2 vertexSize, UVector2 Offset, UINT lodLevel);

	//Check if an update is required.
	void    updateRequired(UVector2 leftTop, UINT brushSize);

	//Update the lod for this chunk. (obsolete when the engine works correctly)
	void    updateLod(const glm::vec3& cameraPos, bool isVisible);

	//Update the terrainMesh if required.
	void    update();
private:
	//Create a new mesh for the heightmap from offset+33.
	void        fillMesh();

	//Update the index data for indexArray index "i" and heightMap index "index".
	void        updateIndexData(int i ,int index, UINT* pIndexArray);

	//Calculates the normal for a vertex at x,z with index il
	glm::vec3 calculateNormal(int x, int z, int i);
	
	DX10SceneManager* m_pSceneManager;
	DX10ShaderSystem* m_pShaderSystem;

	DX10SceneManager* SceneManager() { return m_pSceneManager; }

	//Components required
	RenderModel* dynamicModel;
	//Position of the chunk ifor the renderer
	SceneNode* m_pChunkNode;	

	//4 terrain chunks 1 lod lower then this one.
	TerrainChunk* m_childChunks;
	//Offset of this chunk.
	UVector2 m_Offset;
	//Spacing between two vertices
	UINT m_vertexOffset;
	//Middle point of the 4 chunks that make up this LOD.
	UVector2 m_midPoint;
	//Lod level to determine the offset between vertices
	UINT m_lodLevel;
	
	//Reference to the global heightmap
	UVector2 m_vertexSize;
	float*   m_pHeightArray;

	//Local Mesh
	Vertex*  m_pVertexArray;
	UINT*    m_pIndexArray;

	//Store wether an mesh update is required.
	bool m_bUpdateRequired;
	
	//4 byte mask texture
	Texture  m_maskTexture;   

	//4 textures foreach mask
	Texture texture0;
	Texture texture1;
	Texture texture2;
	Texture texture3;
};

#endif // TERRAINCHUNK_H