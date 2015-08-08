#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <string>
#include <vector>
#include <ostream>

#include "../main/globalfunctions.h"
#include "../main/EventFn.h"
#include "../main/Event.h"
#include "../gui/DebugWND.h"

class IEventListener;

//dispatch and handle events.
static UINT _stdcall handleThread(void* pArgs);
static UINT _stdcall tickThread(void* pArgs);

extern HRESULT postException(HRESULT windowsCode, const char* strMessage, ...);
extern int postException(std::string strException, const char* strMessage, ...);

extern void postMessage(const char* strFormat, ...);

//The main event manager class.
class EventManager
{
public:
	EventManager();
	~EventManager();
	//Singleton
	static EventManager& Instance();
	//Creates the handle thread.
	HRESULT initialize(DebugWND* pDebugWnd);
	//creates the debugWND.
	HRESULT initWindow(HINSTANCE hInstance, int cmdShow);
	//Register new IEventListener(s)
	void Register(IEventListener* pEventListener);
	//dispatch and handle events.
	void handleEvents();

	//Post a windows exception
	HRESULT postException(HRESULT windowsCode, const char* strMessage, ...);
	//Post any exception message
	int postException(std::string strException, const char* strMessage, ...);

	//Post an error without a messageBox
	HRESULT postError(HRESULT hr, const char* strMessage, ...);
	//Post a message without a messageBox
	void    postMessage(const char* strMessage, ...);
	//Post an event.
	void    postEvent(Event e);
private:
	//iterate through all IEventListeners and call handleEvent()
	void fireEvent(Event e);
	//obsolete probaly.
	std::vector<IEventListener*> m_pListenerArray;
	//array of fired events.
	std::vector<Event> m_eventQueue;

	HANDLE m_hHandleThread;
	UINT   m_dwHandleThread;

	DebugWND* m_pDebugWnd;
	
	unsigned long dwThreadId;
	static const unsigned int uMessageLenght = 256;
};

class IEventListener
{
public:	
	virtual void handleEvent(Event& e) = 0;
protected:
	IEventListener(const char* strIdentifier)
	{
		m_strIdentifier = strIdentifier;

		EventManager::Instance().Register(this);
	}
	void postEvent(Event& e)
	{
		EventManager::Instance().postEvent(e);
	}
private:
	const char* m_strIdentifier;
};

#endif