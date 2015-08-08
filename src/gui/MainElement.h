#pragma once
#include "GUIElement.h"

//Main GUIElement provided by the GUInterface class
class MainElement : public GUIElement
{	
public:
	//Doesn't need a parent since its the top layer.
	~MainElement();
	HRESULT Initialize(UVector2 Size);
	
	void onEvent(Event e) {};

	//Unused
	void onMouseMove(UVector2 mousePos){}
	void onMouseEvent(MouseEvent e, UVector2 mousePos) {}
	void onKeyboardEvent(){}
	void onDraw(){}
private:
	
};