#include "UserWND.h"

//For the InputMessage callback
namespace
{
	MessageFn* g_pFunction;
}

UserWND::UserWND(KeybindManager* pManager)
{
	m_hUserWND = 0;
	m_pManager = pManager;
	g_pFunction = bindMessageFn(this, &UserWND::handleMessage);
}

UserWND::~UserWND()
{
	::DestroyWindow(m_hUserWND);

	::UnregisterClass(USERWND_CLASSNAME, NULL);
}

void UserWND::handleMessage(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	char strKey[2];
	strKey[1] = '\0';
	switch(uMessage)
	{
	case WM_SIZE:
		m_windowSize.x = LOWORD(lParam);
		m_windowSize.y = HIWORD(lParam);
		break;
	case WM_KEYDOWN:
		strKey[0] = (char)(wParam);
		m_pManager->commit(strKey, true);
		break;
	case WM_KEYUP:
		strKey[0] = (char)(wParam);
		m_pManager->commit(strKey, false);
		break;
	case WM_MOUSEMOVE:
		m_pManager->commit("mousemove", (UINT)((float)LOWORD(lParam) / (float)m_windowSize.x * 800.0f), (UINT)((float)HIWORD(lParam) / (float)m_windowSize.y * 600.0f));
		break;
	case WM_LBUTTONDOWN:
		m_pManager->commit("lbuttondown", (UINT)((float)LOWORD(lParam) / (float)m_windowSize.x * 800.0f), (UINT)((float)HIWORD(lParam) / (float)m_windowSize.y * 600.0f));
		break;
	case WM_LBUTTONUP:
		m_pManager->commit("lbuttonup", (UINT)((float)LOWORD(lParam) / (float)m_windowSize.x * 800.0f), (UINT)((float)HIWORD(lParam) / (float)m_windowSize.y * 600.0f));
		break;
	}
}
//Creates the window
HRESULT UserWND::initWindow(HINSTANCE hInstance, int cmdShow)
{	
	try
	{
		WNDCLASSEX _Classex;

		_Classex.cbSize = sizeof( WNDCLASSEX );
		_Classex.style  = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		_Classex.lpfnWndProc = UserWNDProcedure;
		_Classex.cbClsExtra  = 0;
		_Classex.cbWndExtra  = 0;
		_Classex.hInstance   = hInstance;
		_Classex.hIcon       = 0;
		_Classex.hCursor     = LoadCursor (NULL, IDC_ARROW);
		_Classex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
		_Classex.lpszMenuName  = NULL;
		_Classex.lpszClassName = USERWND_CLASSNAME;
		_Classex.hIconSm       = 0;

		if(!::RegisterClassEx(&_Classex))
			throw(::GetLastError());
		
		RECT clientRect = { 0,0,800,600 } ;
		::AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, FALSE);

		m_hUserWND = ::CreateWindow(//WS_EX_TRANSPARENT | WS_EX_COMPOSITED,
									 USERWND_CLASSNAME,
									 "UserWND",
									 WS_OVERLAPPEDWINDOW,
									 CW_USEDEFAULT,
									 CW_USEDEFAULT,
									 clientRect.right  - clientRect.left,
									 clientRect.bottom - clientRect.top,
									 0,
									 0, 
									 hInstance,
									 0);
		if(m_hUserWND == NULL)
			throw(::GetLastError());

		::ShowWindow(m_hUserWND, cmdShow);
	}
	catch(DWORD Error)
	{
		::UnregisterClass(USERWND_CLASSNAME, hInstance);

		::CloseWindow(m_hUserWND);

		return Error;
	}

	return S_OK;
}
//Resize the window to a new size
void UserWND::resize(UVector2 Size)
{
	//Get the current desktop
	RECT windowRect;
	GetWindowRect(m_hUserWND, &windowRect);

	RECT clientRect = { 0, 0, Size.x, Size.y };
	::AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, FALSE);

	::MoveWindow(m_hUserWND, 
				 windowRect.left,
				 windowRect.top ,
				 clientRect.right  - clientRect.left,
				 clientRect.bottom - clientRect.top,
				 true);
}
//Returns the HWND of this Window.
HWND UserWND::getHandle()
{
	return m_hUserWND;
}
//Windows callback routine
LRESULT CALLBACK UserWNDProcedure(HWND _hUserWND, 
								  unsigned int Message, 
								  WPARAM wParam,
								  LPARAM lParam)
{	
	HDC hdc;	
    PAINTSTRUCT ps;	

    switch (Message)
    {	
		case WM_CREATE:
			break;	
		case WM_PAINT:		
			hdc = BeginPaint( _hUserWND, &ps );
			EndPaint( _hUserWND, &ps );	
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			break; 
		default:
			if(g_pFunction != NULL)
				(*g_pFunction)(Message, wParam, lParam);
			break;
    }


    return ::DefWindowProc (_hUserWND,
							Message,
							wParam,
							lParam );
}