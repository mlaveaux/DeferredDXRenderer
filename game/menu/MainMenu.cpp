#include "MainMenu.h"

#include <main/EventManager.h>
#include <main/globals.h>

MainMenu::MainMenu(GUIElement* pParent)
{
	setParent(pParent);
}

MainMenu::~MainMenu()
{

}

HRESULT MainMenu::initialize()
{
	setDimensions(UVector2(225, 100), UVector2(350, 470));
	setName("MainMenu");
	
	Font    standardFont;
	Texture fontTexture;

	Surface()->loadTexture(fontTexture, "ui\\standardFont.png");
	if(FAILED(standardFont.initialize("..\\data\\userinterface\\standardfont.ini", fontTexture, 20)))
		return E_FAIL;

	//Initialize buttons (children)	
	m_pStartSPButton.initialize(this, "btnStartSP", "ui\\standardButton.png");
	m_pStartSPButton.setDimensions(UVector2(40, 130), UVector2(250, 50));
	m_pStartSPButton.setText("Singleplayer", standardFont);

	m_pStartMPButton.initialize(this, "btnStartMP", "ui\\standardButton.png");
	m_pStartMPButton.setDimensions(UVector2(40, 130 + 55), UVector2(250, 50));
	m_pStartMPButton.setText("Multiplayer", standardFont);

	m_pEditorButton.initialize(this, "btnEditor", "ui\\standardButton.png");
	m_pEditorButton.setDimensions(UVector2(40, 130 + 110), UVector2(250, 50));
	m_pEditorButton.setText("Editor", standardFont);
	
	m_pOptionsButton.initialize(this, "btnOptions", "ui\\standardButton.png");
	m_pOptionsButton.setDimensions(UVector2(40, 130 + 165), UVector2(250, 50));
	m_pOptionsButton.setText("Options", standardFont);

	m_pQuitButton.initialize(this, "btnQuit", "ui\\standardButton.png");
	m_pQuitButton.setDimensions(UVector2(40, 130 + 220), UVector2(250, 50));
	m_pQuitButton.setText("Quit", standardFont);

	//Initialize background	
	m_sprite.pos        = m_elementPos;
	m_sprite.size       = m_elementSize;
	m_sprite.uLayer     = m_elementLayer; 
	m_sprite.texCoord   = glm::vec2(0.0f, 0.0f);
	m_sprite.texSize    = glm::vec2(1.0f, 1.0f);
	m_sprite.color      = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	Surface()->loadTexture(m_sprite.texture, "ui\\mainMenu.png");

	return S_OK;
}

void MainMenu::onDraw()
{
	Surface()->drawSprite(&m_sprite);
}

void MainMenu::onEvent(Event e)
{
	if(strcmp(e.getID(), m_pStartSPButton.getName())== 0)
	{
		Event load_state("changestate", 2);
		load_state[0] = EventArg("game");
		load_state[1] = EventArg("customgame"); 

		postEvent(load_state);
	}
	else if(strcmp(e.getID(), m_pEditorButton.getName()) == 0)
	{		
		Event load_state("changestate", 2);
		load_state[0] = EventArg("editor");
		load_state[1] = EventArg("customgame"); 

		postEvent(load_state);
	}
	else if(strcmp(e.getID(), m_pQuitButton.getName()) == 0)
	{
		Event quit("quit", 0);

		postEvent(quit);
	}
}