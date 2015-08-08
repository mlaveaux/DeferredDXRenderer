#ifndef FONT_H
#define FONT_H

#include <windows.h>
#include <glm/glm.hpp>

#include "main/Globals.h"
#include "render/DX11/DX11Surface.h"

enum Align
{
	ALIGN_LEFT,
	ALIGN_RIGHT,
	ALIGN_CENTER,
};

struct Text
{
	char*    m_strText;
	UVector2 m_Pos;
	Align    m_Alignment;
};

class Font
{
public:
	int initialize(std::string strInitfile, Texture textureRef, unsigned int uLenght);

	void    drawText(DX11Surface* pSurface, const char* strText, UVector2 Pos, glm::vec4 Color, Align Alignment);
	void    release();
private:
	Sprite*        m_pSpriteArray;
	Texture        m_textureRef;

	unsigned int m_uLenght;

	unsigned short* m_pwCharBuf;
	unsigned short  m_wNumOfElements;
	unsigned short  m_wTotWidth;
};

#endif