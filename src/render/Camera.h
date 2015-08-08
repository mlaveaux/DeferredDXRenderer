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

#ifndef CAMERA_H
#define CAMERA_H

#include "main/Globals.h"
#include "main/EventManager.h"
#include "main/KeybindManager.h"

#include "render/CameraNode.h"

#include <glm/glm.hpp>
#include <DirectXMath.h>

/**
 * Pan the camera in 8 directions. Rotate the camera horizontally and vertically around the view point.
 *
 * TODO:
 * -Add different settings for the camera.
 */
class Camera : public CameraNode, IEventListener
{
public:	
	/**
     * Initializes the CameraNode for usage.
     */
	Camera(UINT sizeX, UINT sizeY, float fov);

	/**
     * @returns a ray from the camera to mouse direction.
     */
    void calculateRay(UVector2 mousePos, glm::vec3& rayOrigin, glm::vec3& rayDirection);

	/**
     * Set the terrain boundaries to move the camera in.
     */
	void setBoundaries(UVector2 boundaries);

	/**
     * Update the camera.
     */
	void interpolate(float fTime);

    /**
     * Handle a camera event.
     */
	void handleEvent(Event& e);

private:
	void Rotate(int x, int y); // perform rotate, move and zoom where the integers indicate directions.
	void Move(int x, int y);
	void Zoom(int i);

	int m_zoomIn;
	int m_zoomOut;

	int m_forward;
	int m_backwards;
	int m_moveleft;
	int m_moveright;

	int m_rotate;
	
	float m_zoom;
    
    glm::vec2 m_newPosMove;
    glm::vec2 m_newPosRotation;
    glm::vec2 m_currentRotation;
    glm::vec3 m_boundaries;
};

#endif // CAMERA_H