#pragma once
/*
#include <windows.h>

#include "render/DX10RenderSystem.h"
#include "render/XMesh.h"
#include "main/Globals.h"

struct EntityState
{
	D3DXVECTOR2 Pos;

	float Speed;
	float Rotation;
};

class IUnit
{
public:
	HRESULT Initialize(DX10RenderSystem* pRenderSystem, XMesh* pMesh, EntityState State);

	EntityState GetState();
	void SetState(EntityState NewState);
	void Interpolate(float timepassed);
	void Render();
private:
	EntityState NewState;
	EntityState CurrentState;
	EntityState OldState;

	D3DXMATRIX World;

	XMesh* pMesh;
};*/