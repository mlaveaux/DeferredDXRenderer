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

#ifndef RENDERDEBUG_H
#define RENDERDEBUG_H

#include "gui/GUIElement.h"
#include "gui/Font.h"
#include "gui/controls/Textbox.h"

//DebugElement which displays render debug information, FPS and RenderCalls.
class DebugElement : public GUIElement
{
public:
	//Default constructor
	DebugElement(GUIElement* pParent);
	~DebugElement();
	int initialize();
	void setRenderCalls(unsigned int uCalls);
	void setFramesPerSecond(unsigned int uFrames);
	void setPolycount(unsigned int uCount);

	void onMouseMove(UVector2 mousePos){};
	void onMouseEvent(MouseEvent e, UVector2 mousePos){};
	void onKeyboardEvent(){};
	void onEvent(Event e){};
	void onDraw();
private:
	Textbox* m_pRenderCalls;
	Textbox* m_pFramesPerSecond;
	Textbox* m_pPolygonCount;
};

#endif // RENDERDEBUG_H