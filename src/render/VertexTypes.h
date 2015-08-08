#ifndef VERTEXTYPE_H
#define VERTEXTYPE_H

#include <glm/glm.hpp>

enum VertexType
{
	VERTEX = 0,
	SPRITE,
	PARTICLE,
};

struct Vertex
{
    glm::vec3 Pos;
	glm::vec2 TexCoord;
	glm::vec3 Normal;

	Vertex(){}
	
	Vertex(glm::vec3 p)
	{	
		Pos      = p;
		TexCoord = glm::vec2(0.0f, 0.0f);
		Normal   = glm::vec3(0.0f, 1.0f, 0.0f);
	}
	
	Vertex(glm::vec3 p, glm::vec2 t)
	{
		Pos      = p;
		TexCoord = t;
		Normal   = glm::vec3(0.0f, 1.0f, 0.0f);
	}	
	
	Vertex(glm::vec3 p, glm::vec2 t, glm::vec3 n)
	{
		Pos      = p;
		TexCoord = t;
		Normal   = n;
	}
};

struct ParticleVertex
{
	glm::vec3 Pos;
	glm::vec2 Size;

	ParticleVertex(glm::vec3 p, glm::vec2 s)
	{
		Pos  = p;
		Size = s;
	}
};

struct SpriteVertex
{
	glm::vec2 TexCoord;
	glm::vec2 TexSize;

	SpriteVertex(){}
		
	SpriteVertex(glm::vec2 tp, glm::vec2 ts)
	{
		TexCoord = tp;
		TexSize  = ts;
	}		
};

#endif