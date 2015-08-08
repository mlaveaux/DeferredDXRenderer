#include "Font.h"

#include <Windows.h>
#include "../main/GlobalFunctions.h"

int Font::initialize(std::string strInitfile, Texture textureRef, unsigned int uLenght)
{
    // load the font data
    m_uLenght = uLenght;
    m_textureRef = textureRef;

    BYTE* pbBuffer = NULL;
    DWORD dwArraySize = 0;

    HRESULT hr = S_OK;

    // load the init file
    hr = loadfile(&pbBuffer, dwArraySize, strInitfile.c_str());
    if (hr != S_OK) {
        return hr;
    }

    // convert the bytes to chars
    char* strFile = (char*)pbBuffer;
    char* strToken = NULL;

    // read the first line for numofElements
    char* strLine = ::strtok_s(strFile, "\n\r", &strToken);

    m_wNumOfElements = ::atoi(strLine);
    m_pwCharBuf = new WORD[m_wNumOfElements];

    // iterate through all the init values of the char width
    UINT i = 0;
    while (strLine != NULL && i != m_wNumOfElements) {
        strLine = ::strtok_s(NULL, "\n\r", &strToken);

        m_pwCharBuf[i] = ::atoi(strLine);
        i++;
    }

    // get the total width
    m_wTotWidth = m_pwCharBuf[m_wNumOfElements - 1];

    // clear the array
    strFile = NULL;
    SAFE_DELETE_ARRAY(pbBuffer);

    // create the dynamic buffer
    m_pSpriteArray = new Sprite[uLenght];

    glm::mat4x4 World(1.0f);

    return S_OK;
}

void Font::drawText(DX11Surface* pSurface, const char* strText, UVector2 Pos, glm::vec4 Color, Align alignment)
{
    //Initialize the variables
    UINT i = 0;
    UINT uIndex = 0;
    UINT vIndex = 0;
    UINT width = 0;

    char ascii = 0;
    float LetterXPos = 0;
    float TexStart = 0.0f;
    float TexEnd = 0.0f;

    //For each letter in the string ALIGN_LEFT
    for (i = 0; i < strlen(strText); ++i) {
        //convert the ascii code to own codes ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
        ascii = strText[i];
        switch(ascii) {
        case 58:
            uIndex = 64;
            break;
        case 46:
            uIndex = 65;
            break;
        case 42:
            uIndex = 66;
            break;
        }

        if ((ascii >= 65) && (ascii <= 90)) {
            uIndex = ascii - 65;
        }
        else if ((ascii >= 97) && (ascii <= 122)) {
            uIndex = ascii - 97 + 26;
        }
        else if ((ascii >= 48) && (ascii <= 57)) {
            uIndex = ascii - 48 + 52;
        }

        //the vertex index
        vIndex = i * 4;

        width = (m_pwCharBuf[uIndex + 1] - m_pwCharBuf[uIndex]);/// 35.0f * _MessageDesc.Height;
        TexStart = (float)(m_pwCharBuf[uIndex] / (float)m_wTotWidth);
        TexEnd = (float)(m_pwCharBuf[uIndex + 1] / (float)m_wTotWidth);

        m_pSpriteArray[i].pos = UVector2(Pos.x + (UINT)LetterXPos, Pos.y);
        m_pSpriteArray[i].size = UVector2(width, 17);
        m_pSpriteArray[i].texCoord = glm::vec2(TexStart, 0.0f);
        m_pSpriteArray[i].texSize = glm::vec2(TexEnd - TexStart, 1.0f);
        m_pSpriteArray[i].color = Color;
        m_pSpriteArray[i].texture = m_textureRef;
        m_pSpriteArray[i].uLayer = 20;

        LetterXPos += width;
    }

    //Move all to the left by a half ALIGN_CENTER
    if (alignment == ALIGN_CENTER) {
        for (i = 0; i < strlen(strText); i++) {
            m_pSpriteArray[i].pos.x -= (UINT)(LetterXPos * 0.5f);
        }
    }

    //Move all to the left by the whole word ALIGN_LEFT
    pSurface->drawSpriteArray(*m_pSpriteArray, (UINT)strlen(strText));
}

void Font::release()
{
    SAFE_DELETE_ARRAY(m_pwCharBuf);
    SAFE_DELETE_ARRAY(m_pSpriteArray);
}