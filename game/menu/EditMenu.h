#pragma once
#include <gui/GUIElement.h>
#include <gui/controls/RadioButton.h>
#include <main/CVarManager.h>

class EditMenu : public GUIElement
{
public:
	EditMenu(GUIElement* pParent);
	~EditMenu();
	HRESULT initialize();

	//GUIElement overrides
	void onMouseMove(UVector2 mousePos) {}
	void onMouseEvent(MouseEvent e, UVector2 mousePos) {}
	void onKeyboardEvent(){}
	void onDraw() {};
	void onEvent(Event e);
private:	
	//Change edit type
	RadioButton* m_terrainEdit;
	RadioButton* m_textureEdit;

	//Smooth edit up and down
	RadioButton* m_pSmoothUpButton;
	RadioButton* m_pSmoothDownButton;

	//Flat edit up and down
	RadioButton* m_pFlatUpButton;
	RadioButton* m_pFlatDownButton;

	//Smoothing and flatting
	RadioButton* m_pSmoothButton;
	RadioButton* m_pFlattenButton;
};