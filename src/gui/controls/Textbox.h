#include "../gui/GUIElement.h"

class Textbox : public GUIElement
{
public:
	Textbox(GUIElement* pParent)
	{
		setParent(pParent);
	}
	~Textbox()
	{

	}
	void initialize(Font Font, unsigned int uTextLenght)
	{
		m_Font = Font;
		m_uTextLenght = uTextLenght;
	}
	void setText(std::string strText)
	{
		m_strText = strText;
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
		m_Font.drawText(Surface(), m_strText.c_str(), m_elementPos, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ALIGN_LEFT);
	}
private:	
	std::string  m_strText;
	unsigned int m_uTextLenght;

	Font m_Font;
};