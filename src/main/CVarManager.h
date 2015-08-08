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

#ifndef CVARSYSTEM_H
#define CVARSYSTEM_H

#include "globals.h"
#include "EventManager.h"

#include <vector>

struct ConVarData;

struct setcvar_struct
{
	std::string strVariable;
	std::string strValue;
};

class CVarManager : public IEventListener
{
public:
	CVarManager();
	~CVarManager();
	static CVarManager& Instance();

	void handleEvent(Event& e);
	void addRef(ConVarData** pConVarData);
	void writeSettings(const char* strFilename);

	void on_setcvar(Event& pArgs);

	ConVarData* search(std::string strName);
private:
	std::vector<ConVarData*> m_pConVarArray;
};

#endif // CVARSYSTEM_H