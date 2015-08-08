#include "ConVar.h"
#include "CVarManager.h"
#include "EventManager.h"

ConVar::ConVar(const char* strName, CValue Value, BYTE Flags, const char* strDescription)
{
	m_pData = NULL;
	m_pData = new ConVarData();

	m_pData->m_strName        = strName;
	m_pData->m_Value          = Value;
	m_pData->m_Flags          = Flags;
	m_pData->m_strDescription = strDescription;

	CVarManager::Instance().addRef(&m_pData);
}

ConVar::ConVar(const char* strName)
{
	m_pData = CVarManager::Instance().search(strName);
}

ConVar::ConVar()
{
	m_pData = NULL;
}

ConVar::~ConVar()
{
	//SAFE_DELETE(m_pData);
}
