#include "GUIElement.h"

#include "..//main//globalfunctions.h"

namespace
{
	void postEvent(Event e)
	{
		EventManager::Instance().postEvent(e);
	}
}

bool CheckBoundaries(UVector2& pointPos, UVector2& squarePos, UVector2& squareSize)
{
	if(pointPos.x > squarePos.x && pointPos.x < squarePos.x + squareSize.x)
	{
		if(pointPos.y > squarePos.y && pointPos.y < squarePos.y + squareSize.y)
		{
			return true;
		}		
	}
	return false;
}

GUIElement::GUIElement()
{
	m_pParent = NULL;
}

GUIElement::~GUIElement()
{
	m_pChildArray.clear();
	
	if(m_pParent != NULL)
	{
		m_pParent->removeChild(this);
		m_pParent      = NULL;
	}
	m_pSurface = NULL;
}

void GUIElement::init(DX11Surface* pSurface)
{
	m_pSurface = pSurface;
}

void GUIElement::CallMouseMove(UVector2 MousePos)
{
	onMouseMove(MousePos);

	if(CheckBoundaries(MousePos, m_elementPos, m_elementSize))
	{
		for(UINT i = 0;i < m_pChildArray.size();i++)
		{
			if(m_pChildArray[i] != NULL)
				m_pChildArray[i]->CallMouseMove(MousePos);
		}
	}
}

void GUIElement::CallMouseEvent(MouseEvent Event, UVector2 MousePos)
{
	onMouseEvent(Event, MousePos);
		
	if(CheckBoundaries(MousePos, m_elementPos, m_elementSize))
	{
		for(UINT i = 0;i < m_pChildArray.size();i++)
		{		
			if(m_pChildArray[i] != NULL)
				m_pChildArray[i]->CallMouseEvent(Event, MousePos);
		}
	}
}

void GUIElement::CallRender()
{
	onDraw();

	for(UINT i = 0;i < m_pChildArray.size();i++)
	{
		if(m_pChildArray[i] != NULL)
			m_pChildArray[i]->CallRender();
	}
}

void GUIElement::setParent(GUIElement* pParent)
{
	m_pParent = pParent;

	pParent->addChild(this);
}

void GUIElement::addChild(GUIElement *pElement)
{
	pElement->init(Surface());
	pElement->setLayer(m_elementLayer+1);
	pElement->onParentMove(UVector2(m_elementPos.x, m_elementPos.y));
	
	m_pChildArray.push_back(pElement);

	pElement->onParentMove(UVector2(m_elementPos.x, m_elementPos.y));
}

void GUIElement::removeChild(GUIElement* pElement)
{
	for(UINT i = 0;i < m_pChildArray.size();i++)
	{
		if(m_pChildArray[i] == pElement)
			m_pChildArray.erase(m_pChildArray.begin() + i);
	}
}

void GUIElement::onParentMove(UVector2 Pos)
{
	m_parentPos = Pos;
}

//*******************************************************
//Protected functions
//*******************************************************
void GUIElement::postEvent(Event e)
{
	::postEvent(e);
}

void GUIElement::postActionSignal(Event e)
{
	if(m_pParent != NULL)
		m_pParent->onEvent(e);
}

void GUIElement::setName(const char* strName)
{
	m_strName = strName;
}

void GUIElement::setLayer(unsigned int uLayer)
{
	m_elementLayer = uLayer;
}

void GUIElement::setDimensions(UVector2 Pos, UVector2 Size)
{
	m_elementPos  = Pos + m_parentPos;
	m_elementSize = Size;

	Event resize("resize", 0);

	onEvent(resize);
}

DX11Surface* GUIElement::Surface()
{
	return m_pSurface;
}