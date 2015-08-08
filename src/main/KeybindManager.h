/****************************************
Class that stores and commits keybind events.
****************************************/
#ifndef KEYBINDMANAGER_H
#define KEYBINDMANAGER_H

#include <string>
#include <array>

#include "Event.h"
#include "EventManager.h"

enum KEYBIND_TYPE
{
	KEYBIND_STAY = 0,
};

class Command
{
public:
	Command() {}
	Command(Event e, KEYBIND_TYPE type) { m_event = e; m_type = type; }  
	Event getEvent() { return m_event; } 
	
	bool         m_pressed;
private:
	Event        m_event;
	KEYBIND_TYPE m_type;
};

class KeybindManager : public IEventListener
{
public:
	KeybindManager();
	//Converts the strKey in a index.
	void bind(std::string strKey, Event e, KEYBIND_TYPE type);
	void commit(std::string strKey, bool pressed);
	void commit(std::string strKey, WORD x, WORD y);

	void handleEvent(Event& e);
private:
	unsigned int getKey(std::string strKey);

	std::array<Command, 256> m_keybindArray;
};

#endif