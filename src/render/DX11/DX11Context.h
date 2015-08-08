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

#ifndef DX11CONTEXT
#define DX11CONTEXT

#include <memory>

class ID3D11Device;
class ID3D11DeviceContext;

/**
 * Storage for the DX11 device and context.
 */
class DX11Context
{
public:
    DX11Context();
    ~DX11Context();

    ID3D11Device* getDevice();

    ID3D11DeviceContext* getContext();

    using Ptr = std::shared_ptr<DX11Context>;
private:
    

    //the device
    ID3D11Device* pd3dDevice;
    ID3D11DeviceContext* pd3dDeviceContext;
};

#endif // DX11CONTEXT