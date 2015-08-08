#include "Entity.h"
/*
HRESULT Entity::Initialize(DX10RenderSystem* pRenderSystem, XMesh* pMesh, EntityState State)
{	
	Entity::pMesh = pMesh;

	OldState = State;
	NewState = State;

	return S_OK;
}

EntityState Entity::GetState()
{
	return CurrentState;
}

void Entity::SetState(EntityState NewState)
{
	OldState = Entity::NewState;

	Entity::NewState = NewState;
}

void Entity::Interpolate(float timepassed)
{
	::D3DXMatrixIdentity(&World);

	D3DXMATRIX RotationMatrix;
	D3DXMATRIX TranslationMatrix;

	//CurrentState.Rotation = OldState.Rotation + (NewState.Rotation - OldState.Rotation) * (timepassed / TICKDELAY);

	::D3DXMatrixRotationY(&RotationMatrix, CurrentState.Rotation);

	//CurrentState.Pos.x = OldState.Pos.x + (NewState.Pos.x - OldState.Pos.x) * (timepassed / TICKDELAY);
	//CurrentState.Pos.y = OldState.Pos.y + (NewState.Pos.y - OldState.Pos.y) * (timepassed / TICKDELAY);

	::D3DXMatrixTranslation(&TranslationMatrix, CurrentState.Pos.x , 0.0f, CurrentState.Pos.y);

	World = RotationMatrix * TranslationMatrix;
}

void Entity::Render()
{
	pMesh->SetMatrix(World);
	pMesh->Render();
}*/