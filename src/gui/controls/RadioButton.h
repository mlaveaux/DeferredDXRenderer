#ifndef RADIOBUTTON_H
#define RADIOBUTTON_H

#include <main/GlobalFunctions.h>
#include <gui/GUIElement.h>
#include <gui/Font.h>

#include "button.h"

#define ACT_UNDEFINED 255

class RadioButton : public GUIElement
{
public:
	RadioButton(GUIElement* pParent, char* strText)
	{
		setParent(pParent);	
		setName(strText);
	}
	~RadioButton()
	{

	}
	void initialize(int iMessage, const char* strFilename)
	{		
		m_iMessage = iMessage;

		m_sprite.pos       = m_elementPos;
		m_sprite.size      = m_elementSize;
		m_sprite.uLayer    = m_elementLayer;
		m_sprite.texCoord  = glm::vec2(0.0f, 0.0f);
		m_sprite.texSize   = glm::vec2(0.333f, 1.0f);
		m_sprite.color     = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		Surface()->loadTexture(m_sprite.texture, strFilename);
	}
	void onMouseMove(UVector2 MousePos)
	{
		if(m_state != BTN_PRESSED)
		{
			if(CheckBoundaries(MousePos, m_elementPos, m_elementSize))
			{
				setState(BTN_HOVER);
			}
			else
			{
				setState(BTN_NORMAL);
			}
		}
	}
	void onMouseEvent(MouseEvent e, UVector2 MousePos)
	{
		if(e == EVT_LBDOWN)
		{
			if(CheckBoundaries(MousePos, m_elementPos, m_elementSize))
				setState(BTN_PRESSED);
			else
				setState(BTN_NORMAL);
		}
		else if(e == EVT_LBUP)
		{
			if(m_state == BTN_PRESSED)
			{
				Event pressed(getName(), 0);

				postActionSignal(pressed);
			}
		}
	}
	void onKeyboardEvent()
	{}
	void onEvent(Event e)
	{}
	void onDraw()
	{
		Surface()->drawSprite(&m_sprite);
	}
	void setState(ButtonState state)
	{
		m_state = state;
		m_sprite.texCoord.x = m_state * 0.333f;
	}
private:
	int         m_iMessage;
	ButtonState m_state;

	Sprite m_sprite;
};

#endif