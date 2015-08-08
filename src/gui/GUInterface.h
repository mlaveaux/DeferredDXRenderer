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

#ifndef GUIINTERFACE_H
#define GUIINTERFACE_H

#include "render/DX11/DX11Surface.h"
#include "main/CVarManager.h"
#include "main/MessageFn.h"
#include "main/globals.h"

#include "MainElement.h"
#include "UserWND.h"

#include <windows.h>
#include <vector>

class GUInterface : public IEventListener
{
public:
	GUInterface();
	~GUInterface();

	HRESULT initialize(DX11Surface* pSurface);	

	void render();
	//GUIHandle    createElement(GUIElement* pElement);
	MainElement* getMainElement();

	//Message handler
	//void handleInput(BYTE KeyboardState[256], IVector2& pMouseMovement);
	//void handleMessage(UINT uMessage, WPARAM wParam, LPARAM lParam);
	void handleEvent(Event& e);
private:
	DX11Surface* m_pSurface;
	
	//The upper GUIElement of the hierarchy
	MainElement m_MainElement;
		
	IVector2 m_MouseOldPos;
	UVector2 m_WindowPos;
};

#endif // GUIINTERFACE_H