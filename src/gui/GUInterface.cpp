#include "GUInterface.h"

#include "../main/globalFunctions.h"
#include "../main/EventManager.h"
#include "../main/ConVar.h"

#include "GUIElement.h"

ConVar ShowGUI(   "gui_show", CValue(true), CVAR_FNONE, "");
//ConVar windowSize("r_windowSize");

GUInterface::GUInterface() : IEventListener("guinterface")
{

}

GUInterface::~GUInterface()
{

}

HRESULT GUInterface::initialize(DX11Surface* pSurface)
{
	m_pSurface = pSurface;

	m_MainElement.init(m_pSurface);

	ConVar r_windowSize = ConVar("r_windowsize");

	if(FAILED(m_MainElement.Initialize(r_windowSize.toVector())))
		return E_FAIL;

	return S_OK;
}

void GUInterface::render()
{
	if(ShowGUI.toBool() == true)
	{
		m_pSurface->Begin();

		m_MainElement.CallRender();

		m_pSurface->End();
	}
}

MainElement* GUInterface::getMainElement()
{
	return &m_MainElement;
}

void GUInterface::handleEvent(Event& e)
{
	if(strcmp(e.getID(), "mousemove") == 0)
	{
		m_MainElement.CallMouseMove(e[0].toVector());
	}
	else if(strcmp(e.getID(), "lbuttondown") == 0)
	{
		m_MainElement.CallMouseEvent(EVT_LBDOWN, e[0].toVector());
	}
	else if(strcmp(e.getID(), "lbuttonup") == 0)
	{
		m_MainElement.CallMouseEvent(EVT_LBUP, e[0].toVector());
	}
}
/*
void GUInterface::handleMessage(UINT uMessage, WPARAM wParam, LPARAM lParam)
{	
	switch(uMessage)
	{
		case WM_WINDOWPOSCHANGING:
		{
			WINDOWPOS* Window = (WINDOWPOS*)lParam;
			m_WindowPos.x = Window->x;
			m_WindowPos.y = Window->y;
			break;
		}
		case WM_MOUSEMOVE:
			m_MainElement.CallMouseMove(UVector2(LOWORD(lParam), HIWORD(lParam)));
			break;
		case WM_LBUTTONDOWN:			
			m_MainElement.CallMouseEvent(EVT_LBDOWN, UVector2(LOWORD(lParam), HIWORD(lParam)));
			break;
		case WM_LBUTTONUP:
			m_MainElement.CallMouseEvent(EVT_LBUP  , UVector2(LOWORD(lParam), HIWORD(lParam)));
			break;
	}
}*/