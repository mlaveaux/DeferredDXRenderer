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

#ifndef DEBUGWND_H
#define DEBUGWND_H

#include "main/MessageFn.h"

#include <windows.h>

#define DEBUGWND_CLASSNAME "DebugWND"

LRESULT CALLBACK debugWNDProcedure(HWND _hDebugWND, 
									unsigned int message, 
									WPARAM wParam, 
									LPARAM lParam);

LRESULT CALLBACK editBoxProcedure(HWND _hEditWND, 
									unsigned int message, 
									WPARAM wParam, 
									LPARAM lParam);

/**
 * Debug Window class that displays the console
 * and receives input string commands.
 */
class DebugWND
{
public:
	DebugWND();
	~DebugWND();
	HRESULT initWindow(HINSTANCE hInstance, int cmdShow);

	void addDebugText(const char* strText);
	void setMessageCallback(MessageFn* pFunction);	
	void handleMessage(UINT uMessage, WPARAM wParam, LPARAM lParam);
private:
	MessageFn* pFunction;

	void ErrorMessage();
	void writeLogfile(const char* strFilename);

	static const unsigned int uCommandLenght = 64;
	static const unsigned int uDebugLenght   = 1024;

	HWND  m_hDebugWND;
	HWND  m_hTextBox;
	HWND  m_hCommandBox;

	static const int m_debugBoxID   = 30000;
	static const int m_commandBoxID = 30001;
};

#endif // DEBUGWND_H