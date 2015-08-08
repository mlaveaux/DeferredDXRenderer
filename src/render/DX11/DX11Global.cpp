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

#include "DX11Global.h"

std::string DXUtil::resolveError(HRESULT code)
{
    switch (code)
    {
    case D3D10_ERROR_FILE_NOT_FOUND:
        return std::string("The file was not found.");
    case D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
        return std::string("There are too many unique instances of a particular type of state object.");
        //case D3DERR_INVALIDCALL:
        //    return std::string("The method call is invalid. For example, a method's parameter may not be a valid pointer.");
        //case D3DERR_WASSTILLDRAWING:
        //	return std::string("The previous blit operation that is transferring information to or from this surface is incomplete.");
    case E_FAIL:
        return std::string("Attempted to create a device with the debug layer enabled and the layer is not installed.");
    case E_INVALIDARG:
        return std::string("An invalid parameter was passed to the returning function.");
    case E_OUTOFMEMORY:
        return std::string("Direct3D could not allocate sufficient memory to complete the call.");
    case S_FALSE:
        return std::string("Alternate success value, indicating a successful but nonstandard completion (the precise meaning depends on context).");
    }

    return std::string("No error occurred");
}