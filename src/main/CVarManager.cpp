#include "CVarManager.h"

#include "ConVar.h"

#include <assert.h>

ConVar sv_cheats("sv_cheats", CValue(false), CVAR_FSERVER, "server value if cheats are enabled");

CVarManager::CVarManager() : IEventListener("cvarmanager")
{

}

CVarManager& CVarManager::Instance()
{
    static CVarManager _CVarManager;
    return _CVarManager;
}

void CVarManager::handleEvent(Event& e)
{
    if (strcmp(e.getID(), "set") == 0) {
        on_setcvar(e);
    }
}

void CVarManager::on_setcvar(Event& pArgs)
{
    std::string strIdentifier = pArgs[0].toChar();//pArgs.strVariable.c_str();
    std::string strValue = pArgs[1].toChar();//.strValue.c_str();

    ConVarData* _pData = search(strIdentifier.c_str());

    if (_pData == NULL) {
        EventManager::Instance().postMessage("cvar %s doesn't exist", strIdentifier.c_str());
        return;
    }

    CValue _value = _pData->m_Value;

    switch (_value.Type) {
    case T_INTEGER:
        _value.Integer = ::atoi(strValue.c_str());
        if (errno > 0) {
            EventManager::Instance().postMessage("can't set cvar %s (expected an integer, but got %s)", strIdentifier.c_str(), strValue.c_str());
            return;
        }
        break;

    case T_FLOAT:
        _value.Float = (float)::atof(strValue.c_str());
        if (errno > 0) {
            EventManager::Instance().postMessage("can't set cvar %s (expected a float, but got %s)", strIdentifier.c_str(), strValue.c_str());
            return;
        }
        break;

    case T_STRING:
        _value.strText = strValue;
        break;

    case T_BOOL:
        if (strValue.compare("true") == 0) {
            _value.Bool = true;
        }
        else if (strValue.compare("false") == 0) {
            _value.Bool = false;
        }
        else {
            EventManager::Instance().postMessage("can't set cvar %s (expected a bool, but got %s)", strIdentifier.c_str(), strValue.c_str());
            return;
        }
        break;

    case T_VECTOR:
        int index = (int)strValue.find_first_of(" ");
        assert(index == strValue.find_first_of(" ")); // Check if index was not truncated.

        std::string strFirst = strValue.substr(0, index);
        std::string strLast = strValue.substr(index + 1, strValue.length() - index);

        _value.Vector.x = ::atoi(strFirst.c_str());
        _value.Vector.y = ::atoi(strLast.c_str());
        if (errno > 0) {
            EventManager::Instance().postMessage("can't set cvar %s (expected a vector, but got %s)", strIdentifier.c_str(), strValue.c_str());
            return;
        }

        break;
    }

    _pData->m_Value = _value;
    if (strValue.length() == 0)
    {
        _pData->m_Value = pArgs[1].getValue();
    }

    Event cvarChange("cvarchange", 1);
    cvarChange[0] = EventArg(_value);
}

void CVarManager::addRef(ConVarData** ppConVarData)
{
    ConVarData* pTempVar = search((*ppConVarData)->m_strName);

    if (pTempVar == NULL) {
        m_pConVarArray.push_back(*ppConVarData);
    }
    else {
        *ppConVarData = pTempVar;
    }
}

ConVarData* CVarManager::search(std::string strName)
{
    for (UINT i = 0; i < m_pConVarArray.size(); i++) {
        if (strName.compare(m_pConVarArray[i]->m_strName) == 0) {
            return m_pConVarArray[i];
        }
    }

    return NULL;
}
