#include "KeybindManager.h"

KeybindManager::KeybindManager() : IEventListener("keybindmanager")
{

}

void KeybindManager::bind(std::string strKey, Event e, KEYBIND_TYPE type)
{
	unsigned int index = getKey(strKey);

	m_keybindArray[index] = Command(e, type);
}

void KeybindManager::commit(std::string strKey, bool pressed)
{
	unsigned int index = getKey(strKey);

	Command& keybind = m_keybindArray[index];
	
	if(pressed != keybind.m_pressed)
	{
		keybind.m_pressed = pressed;

		Event e = keybind.getEvent();

		if(strlen(e.getID()) != 0)
		{
			e[0] = EventArg(pressed);

			postEvent(e);
		}
	}
}

void KeybindManager::commit(std::string strKey, WORD x, WORD y)
{
	Event e(strKey, 1);
	e[0] = EventArg(UVector2((UINT)x, (UINT)y));
	
	postEvent(e);
}

unsigned int KeybindManager::getKey(std::string strKey)
{
	//Convert the strKey in a index.
	/* Layout style:
	 0-25 : alphabet.
	25-35 : digits.
	*/
	BYTE character = strKey[0];
	character -= 97;	

	return character;
}

void KeybindManager::handleEvent(Event& e)
{

}