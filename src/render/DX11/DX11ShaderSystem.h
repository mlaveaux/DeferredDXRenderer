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

#ifndef DX11SHADERSYSTEM
#define DX11SHADERSYSTEM

//Includes
#include "render/vertexTypes.h"
#include "main/globalfunctions.h"
#include "main/eventmanager.h"

#include "DX11Texture.h"
#include "DX11Shader.h"
#include "DX11Context.h"

//Include windows (for HRESULT)
#include <vector>

/**
 * Class that loads and stores shaders.
 */
class DX11ShaderSystem : public IEventListener
{
public:
    DX11ShaderSystem(DX11Context::Ptr context);

    /**
     * Create a shader from a file, with a given type.
     */
	DX11Shader initShader(const char* strShaderName, VertexType type);

    // Implementation of IEventListener
	void handleEvent(Event& e);
private:
	DX11Context::Ptr m_context;
	
	std::vector<DX11Effect> m_pEffectArray; // A list of loaded effects, that can be used inside shaders.
};

#endif // DX11SHADERSYSTEM