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

#ifndef CONVAR_H
#define CONVAR_H

#include "globals.h"

#include <windows.h>
#include <string>

//the different flags of a ConVar
enum Flags
{
	CVAR_FNONE   = 0x00, //No flags, can be edited at any time, although it may have no effect.
	CVAR_FCHEAT  = 0x01, //Cheat flag, can only be changed when sv_cheats is 1.
	CVAR_FCLIENT = 0x02, //Client related variables, needs server validation to change.
	CVAR_FSERVER = 0x04, //Server related variables, can only be change at host.
};

//Type of the CValue
enum CValueType
{
	T_BOOL,
	T_INTEGER,
	T_FLOAT,
	T_STRING,
	T_VECTOR,
	T_VECTOR_3D,
};

struct CValue
{
	//Constructors for each type.
	CValue()               {Bool    = false          ; Type = T_BOOL   ;}
	CValue(bool b)         {Bool    = b              ; Type = T_BOOL   ;}
	CValue(int i)          {Integer = i              ; Type = T_INTEGER;}
	CValue(float f)        {Float   = f              ; Type = T_FLOAT  ;}
	CValue(std::string Text){strText = Text          ; Type = T_STRING ;}
	CValue(UINT x, UINT y) {Vector  = UVector2(x, y) ; Type = T_VECTOR ;}
	CValue(float x, float y, float z) {}

	//All available data types
	//void* m_pData
	bool     Bool;
	int      Integer;
	float    Float;
	std::string strText;
	//long     Long;
	UVector2 Vector;

	//Type in use
	CValueType Type;
};

struct ConVarData
{
	//Unique name
	const char*  m_strName;
	//General description for user/developer
	const char*  m_strDescription;
	//Variable stored (bool, int, float, char*, Vector, Color)
	CValue       m_Value;
	//Flags (NONE, CLIENT, SERVER, CHEAT etc)
	BYTE         m_Flags;
	//Unique Id
	UINT         m_indexId;
};

//ConVar class to manage, change and save all settings and variables
class ConVar
{	
public:
	//Constructor to create new ConVar.
	ConVar(const char* strName, CValue Value, BYTE Flags, const char* strDescription);
	//Constructor to an existing ConVar.
	ConVar(const char* strName);
	//Constructor for an empty ConVar
	ConVar();

	//Decreases the reference counter.
	~ConVar();

	//returns the type
	CValueType  getType() {return m_pData->m_Value.Type;}
	//returns the unique name
	const char* getName() {return m_pData->m_strName;}
	//returns the identifier (ConVarData*)
	long getID() {return (long)m_pData;}

	//Returns the current variable
	bool  toBool ()      {return m_pData->m_Value.Bool;   }
	int   toInt  ()      {return m_pData->m_Value.Integer;}
	float toFloat()	     {return m_pData->m_Value.Float;  }
	const char* toText (){return m_pData->m_Value.strText.c_str();}	
	UVector2 toVector()	 {return m_pData->m_Value.Vector; }
private:
	//ConVar reference
	ConVarData* m_pData;
};

#endif // CONVAR_H