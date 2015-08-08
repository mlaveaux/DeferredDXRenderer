#pragma once
#include <gui/GUIElement.h>
#include <gui/controls/Button.h>

class MainMenu : public GUIElement
{
public:
	MainMenu(GUIElement* pParent);
	~MainMenu();

	HRESULT initialize();

	//GUIElement overrides.
	void onMouseMove(UVector2 mousePos){}
	void onMouseEvent(MouseEvent e, UVector2 mousePos){}
	void onKeyboardEvent(){}
	void onDraw();
	void onEvent(Event e);
private:
	Sprite m_sprite;

	Button m_pStartSPButton;
	Button m_pStartMPButton;
	Button m_pEditorButton;
	Button m_pOptionsButton;
	Button m_pQuitButton;
};