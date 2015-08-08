#ifndef SLIDER_H
#define SLIDER_H

#include "main/GlobalFunctions.h"
#include "main/EventManager.h"
#include "gui/GUIElement.h"

#include "font

class Slider : public GUIElement
{
	Slider(GUIElement* pParent)
	{

	}
	~Slider()
	{

	}
	void initialize()
	{

	}
	
	void onMouseMove(UVector2 mousePos)
	{}
	void onMouseEvent(MouseEvent e, UVector2 mousePos)
	{}
	void onKeyboardEvent()
	{}
	void onEvent(Event e)
	{}
	void onDraw()
	{
		m_Font.drawText(Surface(), m_strText.c_str(), m_elementPos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), ALIGN_LEFT);
	}
};

#endif