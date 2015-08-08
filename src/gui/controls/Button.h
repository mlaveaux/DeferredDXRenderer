/* Copyright 2012-2015 Maurice Laveaux
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BUTTON_H
#define BUTTON_H

#include "../GUIElement.h"
#include "../Font.h"

//State of the button
enum ButtonState
{
    BTN_NORMAL,
    BTN_HOVER,
    BTN_PRESSED,
};

/**
 * Simple button class derived from GUIElement (base)
 * Has a name returned when it's clicked.
 * TODO: Shows a string of text in the middle. (/localized)
 */
class Button : public GUIElement
{
public:
    Button(){}

    void initialize(GUIElement* pParent, const char* strText, const char* strFilename)
    {
        setParent(pParent);
        setName(strText);

        //Set the sprite and load the 
        m_sprite.pos = m_elementPos;
        m_sprite.size = m_elementSize;
        m_sprite.uLayer = m_elementLayer;
        m_sprite.texCoord = glm::vec2(0.0f, 0.0f);
        m_sprite.texSize = glm::vec2(0.333f, 1.0f);
        m_sprite.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        Surface()->loadTexture(m_sprite.texture, strFilename);
    }

    void setText(const char* strText, Font font)
    {
        m_strText = strText;
        m_font = font;
    }

    void onMouseMove(UVector2 MousePos)
    {
        if (CheckBoundaries(MousePos, m_elementPos, m_elementSize)) {
            if (m_state != BTN_PRESSED) {
                setState(BTN_HOVER);
            }
        }
        else {
            setState(BTN_NORMAL);
        }
    }

    void onMouseEvent(MouseEvent e, UVector2 MousePos)
    {
        if (e == EVT_LBDOWN) {
            if (m_state == BTN_HOVER)
                setState(BTN_PRESSED);
        }
        else if (e == EVT_LBUP) {
            if (m_state == BTN_PRESSED) {
                Event clicked(getName(), 0);

                postActionSignal(clicked);
            }
        }
    }

    void onKeyboardEvent()
    {}

    void onEvent(Event e)
    {
        if (strcmp(e.getID(), "resize") == 0) {
            m_sprite.pos = m_elementPos;
            m_sprite.size = m_elementSize;
        }
    }

    void onDraw()
    {
        Surface()->drawSprite(&m_sprite);

        m_font.drawText(Surface(),
            m_strText.c_str(),
            m_elementPos + UVector2((UINT)(m_elementSize.x * 0.5),
            (UINT)(m_elementSize.y * 0.5) - 9),
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
            ALIGN_CENTER);
    }

    void setState(ButtonState state)
    {
        m_state = state;
        m_sprite.texCoord.x = m_state * 0.333f;
    }

private:
    std::string m_strText;
    Font        m_font;
    ButtonState m_state;

    Sprite m_sprite;
};

#endif // BUTTON_H