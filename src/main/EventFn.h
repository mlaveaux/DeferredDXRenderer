#ifndef EVENTFN_H
#define EVENTFN_H

#include "Event.h"

class EventFn
{
public:
	virtual void operator()(EventArg* pArgs){};
};

template<class T>
class EventFnPointer : public EventFn
{
private:
	void (T::*m_pFunction)(EventArg* pArgs);
	T* m_pInstance;
public:
	EventFnPointer(T* instance, void(T::*pFunction)(EventArg* pArgs))
		: m_instance(instance), m_pFunction(pFunction){}
	void operator()(EventArg* pArgs)
	{
		(m_pInstance->*m_pFunction)(pArgs);
	}
};

template<class T>
EventFn* bindEventFn(T* pInstance, void(T::*pFunction)(EventArg* pArgs))
{
	return new EventFnPointer<T>(pInstance, pFunction);
};

#endif
