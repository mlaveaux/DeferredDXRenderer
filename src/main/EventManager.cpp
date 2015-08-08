#include "EventManager.h"

#include "process.h"
#include <dbghelp.h>

HRESULT postException(HRESULT windowsCode, const char* strMessage, ...)
{
	return EventManager::Instance().postException(windowsCode, strMessage);
}

int postException(std::string strException, const char* strMessage, ...)
{
	return EventManager::Instance().postException(strException, strMessage);
}

void postMessage(const char* strFormat, ...)
{
	va_list pArg; 
    va_start(pArg, strFormat); 

	EventManager::Instance().postMessage(strFormat, pArg);
	
    va_end(pArg);
}

EventManager& EventManager::Instance()
{
	static EventManager _EventManager;

	return _EventManager;
}

EventManager::~EventManager()
{
	//::_endthreadex(m_dwHandleThread);
}

HRESULT EventManager::initialize(DebugWND* pDebugWnd)
{	
	m_pDebugWnd     = pDebugWnd;
	//Create the main handle thread
	//m_hHandleThread = (HANDLE)::_beginthreadex(NULL, 0, &handleThread, this, 0, &m_dwHandleThread); 

	return S_OK;
}

//Thread that handles the events
static UINT _stdcall handleThread(void* pArgs)
{	
	EventManager* _pEventManager =  static_cast<EventManager*>(pArgs);

	for(;;)
	{
		//_pEventManager->handleEvents();
	}
}

static UINT _stdcall tickThread(void* pArgs)
{	

}

void EventManager::Register(IEventListener* pEventListener)
{
	m_pListenerArray.push_back(pEventListener);
}

void EventManager::postEvent(Event e)
{
	m_eventQueue.push_back(e);
}

void EventManager::handleEvents()
{
	while(m_eventQueue.size() != 0)
	{
		fireEvent(m_eventQueue[0]);

		m_eventQueue.erase(m_eventQueue.begin());
	}
}

void EventManager::fireEvent(Event e)
{
	for(UINT i = 0;i < m_pListenerArray.size();i++)
	{
		m_pListenerArray[i]->handleEvent(e);
	}
}

HRESULT EventManager::postException(HRESULT windowsCode, const char* strFormat, ...)
{
	va_list pArg; 
    va_start(pArg, strFormat); 

	unsigned int uLenght = _vscprintf(strFormat, pArg) + 1;

	char* strMessage = (char*)malloc(uLenght); 
    int len = vsprintf_s(strMessage, uLenght, strFormat, pArg); 

    va_end(pArg); 

	std::string strError = strMessage;
	
	char* strErrorMessage = NULL;
	//Resolve a windows error.
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
				  FORMAT_MESSAGE_ALLOCATE_BUFFER |
				  FORMAT_MESSAGE_IGNORE_INSERTS,
				  NULL, 
				  windowsCode,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				  (LPTSTR)&strErrorMessage, 
				  0,
				  NULL);
	//If this fails just return the number.
	if ( strErrorMessage == NULL )
	{
		strErrorMessage = new char[uLenght];

		::_itoa_s(windowsCode, strErrorMessage, 12, 10);
	}	

	//Add the error to the DebugWindow
	m_pDebugWnd->addDebugText(strMessage);
	m_pDebugWnd->addDebugText("\r\n");
	m_pDebugWnd->addDebugText("return value: ");
	m_pDebugWnd->addDebugText(strErrorMessage);
	m_pDebugWnd->addDebugText("\r\n");

	strError.append("\n\rreturn value: ");
	strError.append(strErrorMessage);
	
	MessageBox(NULL, strError.c_str(), "Unhandled exception", MB_ICONERROR);

	free(strMessage);
	SAFE_DELETE(strErrorMessage);

	return E_FAIL;
}

int EventManager::postException(std::string strException, const char* strFormat, ...)
{
	va_list pArg; 
    va_start(pArg, strFormat); 

	unsigned int uLenght = _vscprintf(strFormat, pArg) + 1;

	char* strMessage = (char*)malloc(uLenght); 
    int len = vsprintf_s(strMessage, uLenght, strFormat, pArg); 

    va_end(pArg); 

	std::string strError = strMessage;
	
	//Add the error to the DebugWindow
	m_pDebugWnd->addDebugText(strMessage);
	m_pDebugWnd->addDebugText("\r\n");
	m_pDebugWnd->addDebugText("return value: ");
	m_pDebugWnd->addDebugText(strException.c_str());
	m_pDebugWnd->addDebugText("\r\n");

	strError.append("\n\rreturn value: ");
	strError.append(strException);
	
	MessageBox(NULL, strError.c_str(), "Unhandled exception", MB_ICONERROR);

	free(strMessage);

	return E_FAIL;
}

HRESULT EventManager::postError(HRESULT hr, const char* strFormat, ...)
{

	va_list pArg; 
    va_start(pArg, strFormat); 

	unsigned int uLenght = _vscprintf(strFormat, pArg) + 1;

	char* strMessage = (char*)malloc(uLenght); 
    int len = vsprintf_s(strMessage, uLenght, strFormat, pArg); 

    va_end(pArg); 
		
	char* strErrorMessage;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
				  FORMAT_MESSAGE_ALLOCATE_BUFFER |
				  FORMAT_MESSAGE_IGNORE_INSERTS,
				  NULL, 
				  hr,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				  (LPTSTR)&strErrorMessage, 
				  0,
				  NULL);

	if ( strErrorMessage == NULL )
	{
		strErrorMessage = new char[uLenght];
		//strErrorMessage[11] = "\0";

		::_itoa_s(hr, strErrorMessage, 12, 10);
	}
	
	m_pDebugWnd->addDebugText(strMessage);
	m_pDebugWnd->addDebugText("\r\n");
	m_pDebugWnd->addDebugText("return value: ");
	m_pDebugWnd->addDebugText(strErrorMessage);
	m_pDebugWnd->addDebugText("\r\n");
	
	free(strMessage);

	return E_FAIL;
}

void EventManager::postMessage(const char* strFormat, ...)
{
	va_list pArg; 
    va_start(pArg, strFormat); 

    char strMessage[uMessageLenght]; 
    int len = vsprintf_s(strMessage, uMessageLenght, strFormat, pArg); 

    va_end(pArg); 

	m_pDebugWnd->addDebugText(strMessage);
	m_pDebugWnd->addDebugText("\r\n");
}

LONG WINAPI ExceptionFilter(struct _EXCEPTION_POINTERS* pExceptionInfo)
{ 
	//Invoke the MiniDumpWriteDump function 
	typedef BOOL (*PDUMPFUNCTION)(HANDLE hProcess,  
								  DWORD ProcessId,
								  HANDLE hFile,
								  MINIDUMP_TYPE DumpType,
								  PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
								  PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
								  PMINIDUMP_CALLBACK_INFORMATION CallbackParam); 

	//Open the file and load the function pointer from the DbgHelp.dll
    HANDLE hFile = CreateFile("minidump.dmp",
						      GENERIC_READ | GENERIC_WRITE,
							  0,
							  NULL,
							  CREATE_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL,
							  NULL );  
 
    HMODULE hHandle         = ::LoadLibrary("DbgHelp.dll"); 
    PDUMPFUNCTION pWriteDump = (PDUMPFUNCTION)GetProcAddress(hHandle, "MiniDumpWriteDump"); 
 
    if( (hFile == NULL) || (hFile == INVALID_HANDLE_VALUE))  
    {
		//The writing of the error failed, lol
		EventManager::Instance().postException((HRESULT)::GetLastError(), "CreateMiniDump()");

	}
	else
	{
		// Create the minidump 
		MINIDUMP_EXCEPTION_INFORMATION MiniDumpInfo;  
	 
		MiniDumpInfo.ThreadId           = GetCurrentThreadId();  
		MiniDumpInfo.ExceptionPointers  = pExceptionInfo;  
		MiniDumpInfo.ClientPointers     = TRUE;  
	 
		MINIDUMP_TYPE MiniDumpType = MiniDumpNormal;  
	 
		BOOL bResult = (*pWriteDump)(GetCurrentProcess(),
									 GetCurrentProcessId(),
									 hFile,
									 MiniDumpType,
									 &MiniDumpInfo,
									 0,
									 0);  
	}
 
	// Close the file 
	CloseHandle(hFile);  

	return EXCEPTION_EXECUTE_HANDLER;
}

EventManager::EventManager()
{
	::SetUnhandledExceptionFilter(&ExceptionFilter);
}