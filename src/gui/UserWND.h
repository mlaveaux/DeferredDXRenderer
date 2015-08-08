#ifndef USERWND_H
#define USERWND_H

#include <windows.h>
#include <string.h>

#include "main/globals.h"
#include "main/MessageFn.h"
#include "main/KeybindManager.h"

//macro to prevent misspelling.
#define USERWND_CLASSNAME "UserWND"

//static user window callback procedure.
LRESULT CALLBACK UserWNDProcedure(HWND _hUserWND, 
								  unsigned int message, 
								  WPARAM wParam, 
								  LPARAM lParam);

//the main window class.
class UserWND
{
public:		
	UserWND(KeybindManager* pManager);
	~UserWND();
	//Initialize the window.
	HRESULT initWindow(HINSTANCE hInstance, int cmdShow);
	//Resize the window.
	void resize(UVector2 Size);
	//provide the message procedure callback.
	void handleMessage(UINT uMessage, WPARAM wParam, LPARAM lParam);
	//return the HWND
	HWND getHandle();	
private:	
	KeybindManager* m_pManager;
	UVector2        m_windowSize;
	HWND            m_hUserWND;
};

#endif