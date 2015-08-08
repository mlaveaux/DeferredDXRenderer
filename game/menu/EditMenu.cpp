#include "EditMenu.h"

#include <main/ConVar.h>

//ConVar editType("editor_edittype");

EditMenu::EditMenu(GUIElement* pParent)
{
	setParent(pParent);
}

EditMenu::~EditMenu()
{
	//setParent(NULL);
}

HRESULT EditMenu::initialize()
{	
	setDimensions(UVector2(20, 300), UVector2(99, 99));	
	
	m_pSmoothUpButton  = new RadioButton(this, "btnSmoothUp");
	m_pSmoothUpButton->setDimensions(UVector2(0, 0), UVector2(32, 32));
	m_pSmoothUpButton->initialize(0, "ui\\editButton.png");

	m_pSmoothDownButton= new RadioButton(this, "btnSmoothDown");
	m_pSmoothDownButton->setDimensions(UVector2(0, 33), UVector2(32, 32));
	m_pSmoothDownButton->initialize(1, "ui\\editButton.png");

	m_pFlatUpButton    = new RadioButton(this, "btnFlatUp");
	m_pFlatUpButton->setDimensions(UVector2(33, 0), UVector2(32, 32));
	m_pFlatUpButton->initialize(2, "ui\\editButton.png");

	m_pFlatDownButton  = new RadioButton(this, "btnFlatDown");
	m_pFlatDownButton->setDimensions(UVector2(33, 33), UVector2(32, 32));
	m_pFlatDownButton->initialize(3, "ui\\editButton.png");

	m_pSmoothButton    = new RadioButton(this, "btnSmooth");
	m_pSmoothButton->setDimensions(UVector2(66, 0), UVector2(32, 32));
	m_pSmoothButton->initialize(4, "ui\\editButton.png");

	m_pFlattenButton   = new RadioButton(this, "btnFlatten");
	m_pFlattenButton->setDimensions(UVector2(66, 33), UVector2(32, 32));
	m_pFlattenButton->initialize(5, "ui\\editButton.png");

	m_terrainEdit   = new RadioButton(this, "btnTerrain");
	m_terrainEdit->setDimensions(UVector2(0, 66), UVector2(32, 32));
	m_terrainEdit->initialize(6, "ui\\editButton.png");

	m_textureEdit   = new RadioButton(this, "btnTexture");
	m_textureEdit->setDimensions(UVector2(33, 66), UVector2(32, 32));
	m_textureEdit->initialize(7, "ui\\editButton.png");


	return S_OK;
}

void EditMenu::onEvent(Event e)
{
	if(strcmp(e.getID(), "resize") == 0)
		return;

	int editorType = -1;

	if(strcmp(e.getID(), m_terrainEdit->getName()) == 0)
	{
		m_terrainEdit->setState(BTN_PRESSED);
		m_textureEdit->setState(BTN_NORMAL);
		editorType = 0;
	}
	else if(strcmp(e.getID(), m_textureEdit->getName()) == 0)
	{
		m_textureEdit->setState(BTN_PRESSED);
		m_terrainEdit->setState(BTN_NORMAL);
		editorType = 1;
	}

	//Update the editor_type cvar
	Event setEditorCVar("set", 2);
	setEditorCVar[0] = EventArg("editor_type");
	setEditorCVar[1] = EventArg(editorType);

	postEvent(setEditorCVar);

	if(editorType != -1)
		return;

	//Change the group box with 6 buttons
	m_pSmoothUpButton->setState(BTN_NORMAL);
	m_pSmoothDownButton->setState(BTN_NORMAL);

	m_pFlatUpButton->setState(BTN_NORMAL);
	m_pFlatDownButton->setState(BTN_NORMAL);

	m_pSmoothButton->setState(BTN_NORMAL);
	m_pFlattenButton->setState(BTN_NORMAL);

	int edittype = 0;

	if(strcmp(e.getID(), m_pSmoothUpButton->getName()) == 0)
	{
		m_pSmoothUpButton->setState(BTN_PRESSED);
		edittype = 0;
	}
	else if(strcmp(e.getID(), m_pSmoothDownButton->getName()) == 0)
	{
		m_pSmoothDownButton->setState(BTN_PRESSED);
		edittype = 1;
	}
	else if(strcmp(e.getID(), m_pFlatUpButton->getName()) == 0)
	{
		m_pFlatUpButton->setState(BTN_PRESSED);
		edittype = 2;
	}
	else if(strcmp(e.getID(), m_pFlatDownButton->getName()) == 0)
	{
		m_pFlatDownButton->setState(BTN_PRESSED);
		edittype = 3;
	}
	else if(strcmp(e.getID(), m_pSmoothButton->getName()) == 0)
	{
		m_pSmoothButton->setState(BTN_PRESSED);
		edittype = 4;
	}
	else if(strcmp(e.getID(), m_pFlattenButton->getName()) == 0)
	{
		m_pFlattenButton->setState(BTN_PRESSED);
		edittype = 5;
	}
	//Update the terrain_edittype cvar
	Event setTypeCVar("set", 2);
	setTypeCVar[0] = EventArg("terrain_edittype");
	setTypeCVar[1] = EventArg(edittype);
	
	postEvent(setTypeCVar);
}