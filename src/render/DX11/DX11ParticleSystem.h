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

#ifndef DX11PARTICLESYSTEM_H
#define DX11PARTICLESYSTEM_H

#include "DX11Global.h"
#include "DX11SceneManager.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class ParticleEmitter
{
	glm::vec3 position;

	Texture  diffuseTex;
};

class DX11ParticleSystem
{
public:
	HRESULT Initialize(DX11SceneManager* pSceneManager);

private:
	DX11SceneManager* m_pSceneManager;
	DX11ShaderSystem* m_pShaderSystem;

	DX11Shader particleShader;
};

#endif // DX11PARTICLESYSTEM_H