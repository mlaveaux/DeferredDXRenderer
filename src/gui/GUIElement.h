/* Copyright 2012-2015 Maurice Laveaux
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUIELEMENT_H
#define GUIELEMENT_H

#include "render/DX11/DX11Surface.h"
#include "main/EventManager.h"
#include "main/globals.h"

#include <string>
#include <vector>

enum MouseEvent
{
	EVT_LBDOWN,
	EVT_LBUP,
};

//Check if a point lies within the boundaries of a square
extern bool CheckBoundaries(UVector2& pointPos, UVector2& squarePos, UVector2& squareSize); 

//Every GUInterface element is derived from this class.
class GUIElement
{
public:
	//Override always to cleanup memory !!!!.
	GUIElement();
	virtual ~GUIElement();

	//Standard functions
	void init(DX11Surface* pSurface);
	void CallMouseMove(UVector2 MousePos);
	void CallMouseEvent(MouseEvent Event, UVector2 MousePos);
	void CallRender();
	void CallRelease();
	void setDimensions(UVector2 Pos, UVector2 Size);
	void onParentMove(UVector2 Pos);

	const char* getName() { return m_strName.c_str(); }

	//Should be overrided by the derived class when needed.
	virtual void onMouseMove(UVector2 MousePos) = 0;
	virtual void onMouseEvent(MouseEvent Event, UVector2 MousePos) = 0;
	virtual void onKeyboardEvent() = 0;
	virtual void onEvent(Event e) = 0;
	virtual void onDraw() = 0;
protected:
	//Can be used by the derived class.
	void postEvent(Event e);
	void postActionSignal(Event e);
	void setName(const char* strName);
	void setLayer(unsigned int uIndex);
	void setParent(GUIElement* pParent);

	void addChild(GUIElement* pElement);
	void removeChild(GUIElement* pElement);

	DX11Surface* Surface();

	UVector2 m_elementPos;
	UVector2 m_elementSize;
	unsigned int m_elementLayer;
private:
	//Only required by the GUIElement class.
	GUIElement*              m_pParent;
	std::vector<GUIElement*> m_pChildArray;

	DX11Surface* m_pSurface;
	std::string  m_strName;
	UVector2     m_parentPos;
};

#endif // GUIELEMENT_H