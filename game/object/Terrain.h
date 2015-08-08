//********************************************************
//Class that handles the terrain and water in the world.
//********************************************************
#ifndef TERRAIN_H
#define TERRAIN_H

#include "main/globals.h"
#include "main/globalFunctions.h"

#include "main/EventManager.h"
#include "terrainchunk.h"
#include "water.h"

#define	EDIT_SMOOTHUP   = 0,
#define	EDIT_SMOOTHDOWN = 1,
#define	EDIT_FLATUP     = 3,
#define	EDIT_FLATDOWN   = 4,	
#define	EDIT_FLATTEN    = 5,
#define	EDIT_SMOOTHEN   = 6,

struct TerrainHeader
{
	//[terraininfo]
	char*    strName;
	UVector2 vertexSize;
	//[textures]
	char* terrainTexture0;
	char* terrainTexture1;
	char* terrainTexture2;
	char* terrainTexture3;
	//[heightmap]
	float* m_pHeightArray;
};

class Terrain : public IEventListener
{
public:
	Terrain(DX10SceneManager* pSceneManager);
	~Terrain();

	//Create a new flat terrain.
	HRESULT create(UVector2 TerrainSize);
	//Load the terrain from a .world file
	HRESULT load(const char* strFilename);
	//Saves the current terrain to a .world file
	HRESULT save(const char* strFilename);

	//Intersection point calculation.
    bool  intersect(glm::vec3 RayOrigin, glm::vec3 RayDirection, glm::vec3& pHitPos);

	//Get height at a certain pos.
	float getMinHeight(glm::vec2 Pos);

	//Terrain edit functions for explosions or terrain editing(use events instead)
	void editTerrain(glm::vec2 Pos, float fTime, float fStrenght, float fBrushSize, int iType);
    void editTexture(glm::vec2 Pos, float fTime, float fStrenght, float fBrushSize, int iType);
	
	//Manually update the lod, engine should take this role.
	void updateLod(glm::vec3 cameraPos);
	//Update the TerrainChunk meshes according to the editTerrain update.
	void update();

	//Handle engine events, which is currently not used.
	void handleEvent(Event& e);
private:
	DX10SceneManager* m_pSceneManager;

	//Terrain header, terrainSize, total number of chunks (of size 128*128), 
	UVector2 m_vertexSize;
	UVector2 m_numberOfChunks;
	UINT     m_chunkSize;
	//Array of the TerrainChunks, at lowest LOD.
	TerrainChunk*  m_pChunkArray;
	//Array of vertexSize*vertexSize that contains the heightvalues.
	float*         m_pHeightArray;
	//Array of vertexSize*vertexSIze that contains true/false if the vertex can be transversed.
	bool*          m_pPathMap;

	//Add water objects to the world.
	Water m_water;
	
	//Normalmap texture, for all lods to minimize artifacts. This is not used yet.
	Texture normalTexture;
	//Texture of the 4-byte mask.
	Texture m_maskTexture;
};

#endif