#include <windows.h>

#include "logic/Game.h"

int WINAPI WinMain (HINSTANCE hInstance, 
					HINSTANCE hPrevInst,
					LPSTR cmdParam, 
					int cmdShow)
{	
	Game Game;

	if(FAILED(Game.Initialize(hInstance, cmdShow)))
		return E_FAIL;

	MSG msg = {0};	

	while( msg.message != WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
			TranslateMessage( &msg );
			DispatchMessage( &msg );	
        }
		else
		{
			Game.interpolate();
		}
    }

    return ( int )msg.wParam;
}
