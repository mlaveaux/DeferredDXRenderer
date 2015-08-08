#include <sstream>

#include "DebugElement.h"
#include "../main/ConVar.h"

ConVar r_ShowDebug  ("r_showdebug", CValue(false), CVAR_FNONE, "enable the renderdebug window.");

DebugElement::DebugElement(GUIElement* pParent)
{
	setParent(pParent);
}

DebugElement::~DebugElement()
{
	SAFE_DELETE(m_pRenderCalls);
	SAFE_DELETE(m_pFramesPerSecond);
	SAFE_DELETE(m_pPolygonCount)
}

int DebugElement::initialize()
{
	setDimensions(UVector2(0,0), UVector2(100, 20));

	Font    standardFont;
	Texture fontTexture;

	Surface()->loadTexture(fontTexture, "ui/standardFont.png");
	if(FAILED(standardFont.initialize("../data/userinterface/standardfont.ini", fontTexture, 20)))
		return E_FAIL;

	m_pRenderCalls  = new Textbox(this);
	m_pRenderCalls->initialize(standardFont,20);
	m_pRenderCalls->setDimensions(UVector2(20, 20), UVector2(0, 0));

	m_pFramesPerSecond = new Textbox(this);
	m_pFramesPerSecond->initialize(standardFont, 20);
	m_pFramesPerSecond->setDimensions(UVector2(20, 40), UVector2(0, 0));

	m_pPolygonCount = new Textbox(this);
	m_pPolygonCount->initialize(standardFont, 20);
	m_pPolygonCount->setDimensions(UVector2(20, 60), UVector2(0, 0));

	return 0;
}

void DebugElement::onDraw()
{

}

void DebugElement::setRenderCalls(unsigned int uCalls)
{
	std::stringstream stream("");
	stream << "drawCalls: ";
	stream << uCalls;

    m_pRenderCalls->setText(stream.str());
}

void DebugElement::setFramesPerSecond(unsigned int uFrames)
{
	std::stringstream stream("");
	stream << "fps: ";
	stream << uFrames;
	
	m_pFramesPerSecond->setText(stream.str());
}

void DebugElement::setPolycount(unsigned int uCount)
{
	std::stringstream stream("");
	stream << "polyCount: ";
	stream << uCount;
	
	m_pPolygonCount->setText(stream.str());
}

