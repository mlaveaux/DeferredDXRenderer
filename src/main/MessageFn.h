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

#ifndef H_MESSAGEFN
#define H_MESSAGEFN

#include <Windows.h>

typedef unsigned int UINT;

class MessageFn
{
public:
	virtual void operator()(UINT p1, WPARAM p2, LPARAM p3){};
};

template<class T>
class MessageFnPointer : public MessageFn
{
private:
	void (T::*_pFunction)(UINT, WPARAM, LPARAM);
	T*   _pInstance;
public:
	MessageFnPointer(T* pInstance, void(T::*pFunction)(UINT, WPARAM, LPARAM)) : _pInstance(pInstance), _pFunction(pFunction)
	{}
	void operator()(UINT p1, WPARAM p2, LPARAM p3)
	{
		(_pInstance->*_pFunction)(p1, p2, p3); 	
	}	
};

template<class T>
MessageFn* bindMessageFn(T* pInstance, void(T::*pFunction)(UINT, WPARAM, LPARAM))
{
	return new MessageFnPointer<T>(pInstance, pFunction);
}

#endif // H_MESSAGEFN