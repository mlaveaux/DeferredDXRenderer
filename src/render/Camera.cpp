#include "Camera.h"

#include "../main/ConVar.h"

ConVar panSens("pansensitivity", CValue(50.0f), CVAR_FNONE, "");
ConVar zoomSens("zoomsensitivity", CValue(50.0f), CVAR_FNONE, "");
ConVar rotateSens("rotatesensivity", CValue(50.0f), CVAR_FNONE, "");

//Command moveleft( "+moveleft" , EVENT_STAY);
//Command moveright("+moveright", EVENT_STAY);

#define PANSENS_FACTOR 0.05f
#define ZOOMSENS_FACTOR 0.0005f
#define ROTATESENS_FACTOR 0.0002f

Camera::Camera(UINT sizeX, UINT sizeY, float fov) : IEventListener("camera"), CameraNode(sizeX, sizeY, fov)
{
    m_upPos = glm::vec3(0.0f, 1.0f, 0.0f);
    m_atPos = glm::vec3(0.0f, 0.0f, 0.0f);
    m_eyePos = glm::vec3(0.0f, 0.0f, -15.0f);

    m_newPosMove = glm::vec2(0.0f, 0.0f);
    m_zoom = 10.0f;

    m_forward = false;
    m_backwards = false;
    m_moveleft = false;
    m_moveright = false;

    m_zoomIn = false;
    m_zoomOut = false;

    m_rotate = false;

    m_boundaries.x = 128.0f;
    m_boundaries.y = 128.0f;

    m_currentRotation = glm::vec2(0.0f, 0.9f);
    m_newPosRotation = glm::vec2(0.0f, 0.9f);
}

void Camera::setBoundaries(UVector2 boundaries)
{
    m_boundaries.x = (float)boundaries.x;
    m_boundaries.y = (float)boundaries.y;
}

void Camera::calculateRay(UVector2 mousePos, glm::vec3& rayOrigin, glm::vec3& rayDirection)
{
    glm::vec3 rayScreen;

    glm::mat4x4 inverseViewMatrix = glm::inverse(m_viewMatrix);

    rayScreen.x = (((2.0f * mousePos.x) / m_size.x) - 1) / m_projMatrix[1][1];
    rayScreen.y = -(((2.0f * mousePos.y) / m_size.y) - 1) / m_projMatrix[2][2];
    rayScreen.z = 1.0f;

    //D3DXVec3Transform(&rayDirection4, &rayScreen, &inverseViewMatrix);

    //rayDirection = (D3DXVECTOR3)rayDirection4;		
    rayDirection.x = rayScreen.x * inverseViewMatrix[0][0] + rayScreen.y * inverseViewMatrix[1][0] + rayScreen.z * inverseViewMatrix[2][0];
    rayDirection.y = rayScreen.x * inverseViewMatrix[0][1] + rayScreen.y * inverseViewMatrix[1][1] + rayScreen.z * inverseViewMatrix[2][1];
    rayDirection.z = rayScreen.x * inverseViewMatrix[0][2] + rayScreen.y * inverseViewMatrix[1][2] + rayScreen.z * inverseViewMatrix[2][2];

    rayOrigin.x = inverseViewMatrix[3][0];
    rayOrigin.y = inverseViewMatrix[3][1];
    rayOrigin.z = inverseViewMatrix[3][2];
}

void Camera::interpolate(float time)
{
    //Determine if WASD moves or rotates
    if (m_rotate)
    {
        Rotate((int)(m_moveright - m_moveleft), (int)(m_forward - m_backwards));
    }
    else
    {
        Move((int)(m_moveright - m_moveleft), (int)(m_forward - m_backwards));
    }

    m_zoom -= (int)(m_zoomIn - m_zoomOut) * ZOOMSENS_FACTOR * zoomSens.toFloat() * time;

    //Add the NewPosMove for panning camera	
    m_atPos.x += m_newPosMove.x * time * 0.004f * (m_zoom / 5.0f);
    m_atPos.z += m_newPosMove.y * time * 0.004f * (m_zoom / 5.0f);

    if (m_atPos.x < 0.0f)
        m_atPos.x = 0.01f;
    else if (m_atPos.x > m_boundaries.x)
        m_atPos.x = m_boundaries.x - 0.01f;
    if (m_atPos.z < 0.0f)
        m_atPos.z = 0.01f;
    else if (m_atPos.z > m_boundaries.y)
        m_atPos.z = m_boundaries.y - 0.01f;

    //Calculate new angles
    m_currentRotation.x += (m_newPosRotation.x - m_currentRotation.x) * (time * 0.01f);
    m_currentRotation.y += (m_newPosRotation.y - m_currentRotation.y) * (time * 0.01f);

    glm::mat4x4 rotationMatrix(1.0f);

    m_eyePos = glm::vec3(0.01f, 0.0f, -m_zoom);

    glm::vec4 eyeNew;

    //Apply horizontal rotation
    glm::rotate(rotationMatrix, m_currentRotation.y, glm::vec3(1.0f, 0.0f, 0.0f));
    eyeNew = glm::vec4(m_eyePos, 1.0f) * rotationMatrix;

    //Apply verticle rotation
    glm::rotate(rotationMatrix, m_currentRotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
    eyeNew = eyeNew * rotationMatrix;

    m_eyePos = glm::vec3(eyeNew.x, eyeNew.y, eyeNew.z);
    m_eyePos += m_atPos;
}

void Camera::handleEvent(Event& e)
{
    //Read the commands.
    if (strcmp(e.getID(), "moveleft") == 0)
    {
        m_moveleft = e[0].toBool();
    }
    else if (strcmp(e.getID(), "moveright") == 0)
    {
        m_moveright = e[0].toBool();
    }
    else if (strcmp(e.getID(), "forward") == 0)
    {
        m_forward = e[0].toBool();
    }
    else if (strcmp(e.getID(), "backwards") == 0)
    {
        m_backwards = e[0].toBool();
    }
    else if (strcmp(e.getID(), "zoomout") == 0)
    {
        m_zoomOut = e[0].toBool();
    }
    else if (strcmp(e.getID(), "zoomin") == 0)
    {
        m_zoomIn = e[0].toBool();
    }
    else if (strcmp(e.getID(), "rotate") == 0)
    {
        m_rotate = e[0].toBool();
    }
}

void Camera::Rotate(int x, int y)
{
    m_newPosRotation.y += y * rotateSens.toFloat() * ROTATESENS_FACTOR;

    if (m_newPosRotation.y > 1.55f)
    {
        m_newPosRotation.y = 1.55f;
    }
    if (m_newPosRotation.y < 0.2f)
    {
        m_newPosRotation.y = 0.2f;
    }

    m_newPosRotation.x += x * rotateSens.toFloat() * ROTATESENS_FACTOR;

    if (m_newPosRotation.x > (2 * 3.14f))
    {
        m_newPosRotation.x -= 2 * 3.14f;
        m_currentRotation.x -= 2 * 3.14f;
    }
    if (m_newPosRotation.x < -(2 * 3.14f))
    {
        m_newPosRotation.x += 2 * 3.14f;
        m_currentRotation.x += 2 * 3.14f;
    }
}

void Camera::Move(int x, int y)
{
    glm::vec2 TempPosMove;

    TempPosMove.x = (m_atPos.x - m_eyePos.x) * y;
    TempPosMove.y = (m_atPos.z - m_eyePos.z) * y;

    TempPosMove.x += (m_atPos.z - m_eyePos.z) * x;
    TempPosMove.y -= (m_atPos.x - m_eyePos.x) * x;

    TempPosMove = glm::normalize(TempPosMove);

    m_newPosMove.x = TempPosMove.x * panSens.toFloat() * PANSENS_FACTOR;
    m_newPosMove.y = TempPosMove.y * panSens.toFloat() * PANSENS_FACTOR;
}

void Camera::Zoom(int i)
{
}
