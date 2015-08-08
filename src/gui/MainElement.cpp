#include "MainElement.h"

#include "..//main//globalFunctions.h"

MainElement::~MainElement()
{

}

HRESULT MainElement::Initialize(UVector2 Size)
{
	setName("MainElement");
	setLayer(0);

	m_elementPos = UVector2(0, 0);
	m_elementSize = Size;

	return S_OK;
}
/*
GUIElement* MainElement::set(GUIElement* pElement)
{
	removeChild(0);

	return addChild(pElement);
}*/