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

#ifndef CAMERANODE_H
#define CAMERANODE_H

#include "main/globals.h"

#include <windows.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <math.h>

class CameraNode
{
public:
	CameraNode(){}

	CameraNode(UINT w, UINT h, float fov)
	{	
		init(w,h,fov);
	}

	virtual void interpolate(float fTime){}

	void init(UINT w, UINT h, float fov)
	{		
		m_far = 200.0f;

		float aspectRatio = (float)w/(float)h;
		float fovRadians  = glm::radians(fov);

        m_projMatrix = glm::perspective(aspectRatio, fovRadians, 1.0f, m_far);
        
		m_eyePos = glm::vec3(1.0f, 0.0f, 0.0f);
		m_atPos  = glm::vec3(0.0f, 0.0f, 0.0f);
		m_upPos  = glm::vec3(0.0f, 1.0f, 0.0f);

		m_size   = UVector2(w, h);
	}

	void setPosition(float x, float y, float z)
	{
		m_eyePos.x = x;		
		m_eyePos.y = y;		
		m_eyePos.z = z;
	}
	void setLookAt(float x, float y, float z)
	{
		m_atPos.x = x;
		m_atPos.y = y;
		m_atPos.z = z;
	}

	//Retrieve all private information
    glm::mat4x4 getViewMatrix()
	{
		return glm::lookAt(m_eyePos, m_atPos, m_upPos);
	}

    glm::mat4x4 getProjMatrix() { return m_projMatrix; }
    glm::vec3 getEye() { return m_eyePos; }
    glm::vec3 getAt() { return m_atPos; }
	float getViewDistance() { return m_far; }
protected:
    glm::vec3 m_eyePos;
    glm::vec3 m_atPos;
    glm::vec3 m_upPos;

    glm::mat4x4 m_viewMatrix;
    glm::mat4x4 m_projMatrix;

	UVector2   m_size;
	float      m_far;
};

#endif // CAMERANODE_H