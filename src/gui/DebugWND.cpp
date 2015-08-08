#include "DebugWND.h"

//#include "../main/globalFunctions.h"
#include "../main/EventManager.h"

namespace
{
    FAR WNDPROC defEditProcedure;
    MessageFn* g_pFunction;

    void postEvent(Event e)
    {
        EventManager::Instance().postEvent(e);
    }

}

DebugWND::DebugWND()
{
    m_hDebugWND = NULL;
    g_pFunction = NULL;

    g_pFunction = bindMessageFn(this, &DebugWND::handleMessage);
}

DebugWND::~DebugWND()
{
    ::CloseWindow(m_hDebugWND);

    ::CloseWindow(m_hTextBox);
    ::CloseWindow(m_hCommandBox);

    ::UnregisterClass(DEBUGWND_CLASSNAME, NULL);

    writeLogfile("logfile.txt");
}

HRESULT DebugWND::initWindow(HINSTANCE hInstance, int cmdShow)
{
    try
    {
        WNDCLASSEX _Classex;
        _Classex.cbSize = sizeof(WNDCLASSEX);
        _Classex.style = CS_HREDRAW | CS_VREDRAW;
        _Classex.lpfnWndProc = debugWNDProcedure;
        _Classex.cbClsExtra = 0;
        _Classex.cbWndExtra = 0;
        _Classex.hInstance = hInstance;
        _Classex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        _Classex.hCursor = LoadCursor(NULL, IDC_ARROW);
        _Classex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        _Classex.lpszMenuName = NULL;
        _Classex.lpszClassName = DEBUGWND_CLASSNAME;
        _Classex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

        if (!::RegisterClassEx(&_Classex))
            throw(GetLastError());

        m_hDebugWND = ::CreateWindow(DEBUGWND_CLASSNAME,
            "Debug Window",
            WS_OVERLAPPED,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            400,
            315,
            0,
            0,
            hInstance,
            0);
        if (m_hDebugWND == NULL)
            throw(GetLastError());

        m_hTextBox = ::CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL | ES_READONLY, 5, 5, 390, 250, m_hDebugWND, (HMENU)m_debugBoxID, hInstance, 0);
        if (m_hTextBox == NULL)
            throw(GetLastError());

        m_hCommandBox = ::CreateWindow("EDIT", "", WS_CHILD | WS_BORDER, 5, 260, 375, 20, m_hDebugWND, (HMENU)m_commandBoxID, hInstance, 0);
        if (m_hCommandBox == NULL)
            throw(GetLastError());

        HFONT Font;

        Font = CreateFont(-12, 0, 0, 0, FW_DONTCARE, false, false, false, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

        SendMessage(m_hTextBox, WM_SETFONT, WORD(Font), MAKELPARAM(false, 0));
        SendMessage(m_hCommandBox, WM_SETFONT, WORD(Font), MAKELPARAM(false, 0));

        ShowWindow(m_hDebugWND, cmdShow);

        defEditProcedure = (WNDPROC)GetWindowLongPtr(m_hCommandBox, GWLP_WNDPROC);
        SetWindowLongPtr(m_hCommandBox, GWLP_WNDPROC, (long)editBoxProcedure);

        ShowWindow(m_hTextBox, cmdShow);
        ShowWindow(m_hCommandBox, cmdShow);
    }
    catch (DWORD Error)
    {
        UnregisterClass(DEBUGWND_CLASSNAME, NULL);

        CloseWindow(m_hDebugWND);

        CloseWindow(m_hTextBox);
        CloseWindow(m_hCommandBox);

        return Error;
    }

    return S_OK;
}

void DebugWND::addDebugText(const char* strText)
{
    char DestText[uDebugLenght];

    ::GetWindowText(m_hTextBox, DestText, uDebugLenght);

    if ((strlen(strText) + strlen(DestText)) <= uDebugLenght)
        ::strcat_s(DestText, uDebugLenght, strText);
    else
        ::memset(DestText, 0, uDebugLenght);

    ::SetWindowText(m_hTextBox, DestText);

    ::SendMessage(m_hTextBox, WM_VSCROLL, SB_BOTTOM, NULL);
}

void DebugWND::handleMessage(UINT Message, WPARAM wParam, LPARAM lParam)
{
    char strCommand[uCommandLenght];
    ::GetWindowText(m_hCommandBox, strCommand, uCommandLenght);
    ::SetWindowText(m_hCommandBox, "");

    Event commandEvent("command", 1);
    commandEvent[0] = EventArg(strCommand);

    postEvent(commandEvent);

    addDebugText(strCommand);
    addDebugText("\r\n");
}

void DebugWND::writeLogfile(const char* strFilename)
{
    char DestText[uDebugLenght];

    ::GetWindowText(m_hTextBox, DestText, uDebugLenght);

    if (FAILED(writefile((BYTE*)DestText, uDebugLenght, strFilename)))
        return;
}


LRESULT CALLBACK debugWNDProcedure(HWND _hDebugWND,
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
        hdc = BeginPaint(_hDebugWND, &ps);
        EndPaint(_hDebugWND, &ps);
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;
    }

    return ::DefWindowProc(_hDebugWND,
        Message,
        wParam,
        lParam);
}

//editBox event callback.
LRESULT CALLBACK editBoxProcedure(HWND _hEditBox,
    unsigned int Message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (Message)
    {
    case WM_CHAR:
        if (wParam == VK_RETURN) {
            (*g_pFunction)(0, 0, 0);

            return 0;
        }
        break;
    }
    //Call the rest of the default procedure
    return CallWindowProc((WNDPROC)defEditProcedure,
        _hEditBox,
        Message,
        wParam,
        lParam);
}

