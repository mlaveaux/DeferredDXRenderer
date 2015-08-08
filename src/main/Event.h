#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <vector>
#include <memory>

#include "main/globalFunctions.h"
#include "main/ConVar.h"

class EventArg
{
public:
    EventArg()
    {}

    EventArg(std::string strText) : value(strText){}

    EventArg(const char* strText)
    {
        std::string strValue(strText);
        value = CValue(strValue);
    }

    EventArg(long l) : value(l) {}

    EventArg(int  i) : value(i) {}

    EventArg(bool b) : value(b) {}

    EventArg(UVector2 u) : value(u.x, u.y){}
    EventArg(CValue v) : value(v){}

    ~EventArg()
    {}

    const char* toChar() { return value.strText.c_str(); }
    BOOL  toBool()		 { return value.Bool; }
    int   toInt()		 { return value.Integer; }
    UVector2 toVector()  { return value.Vector; }
    CValue getValue() { return value; }

    void operator = (EventArg& arg)
    {
        value = arg.getValue();
    }
private:
    CValue value;
};

// class of an event
class Event
{
public:
    Event() {}

    //Creates an event.
    Event(std::string strName, UINT numOfArguments)
    {
        m_strIdentifier = strName;
        m_args = std::shared_ptr<std::vector<EventArg>>(new std::vector<EventArg>);
        m_args->resize(numOfArguments);
    }

    ~Event(){}

    //Array that holds all the eventArgs for this event.
    //Returns the string identifier of the event.
    const char* getID() { return m_strIdentifier.c_str(); }

    //Returns the arguments
    EventArg& operator[] (UINT index)
    {
        return (*m_args)[index];
    }

private:
    std::string m_strIdentifier;
    std::shared_ptr<std::vector<EventArg>> m_args;
};

#endif // EVENT_H