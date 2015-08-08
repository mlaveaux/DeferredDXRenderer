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

#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include "main/globalfunctions.h"
#include "main/EventManager.h"

#include <windows.h>
#include <xaudio2.h>
#include <X3DAudio.h>

//Struct including the waveformat and the samples
struct SoundData
{	
	SoundData()
	{
		pbData = NULL;
	}

	~SoundData()
	{
		SAFE_DELETE_ARRAY(pbData);
	}

	std::string   m_fileName;
	WAVEFORMATEX  pwfx;
	BYTE*		  pbData;
	DWORD		  dwNumOfBytes;
};

enum EmitterType
{
	EMITTER_REPEAT,
};

//Struct for the emitter of a sound
struct SoundEmitter
{
	SoundEmitter()
	{
		::ZeroMemory(&X3DEmitter     , sizeof(X3DAUDIO_EMITTER));
		::ZeroMemory(&X3DDSPSettings , sizeof(X3DAUDIO_DSP_SETTINGS));
		Active = false;
		pSourceVoice = NULL;
	}

	IXAudio2SourceVoice* pSourceVoice;

	X3DAUDIO_EMITTER      X3DEmitter;
	X3DAUDIO_DSP_SETTINGS X3DDSPSettings;

	EmitterType type;
	UINT uLenght;
	bool Active;
};

//load a wave formatted file (.wav)
extern HRESULT LoadSound(SoundData& pSound, char* Filename);

/**
 * Audio output handler: manages all emitters and sounds, manages all listeners.
 */
class AudioSystem : public IEventListener
{
public:
	AudioSystem();
	HRESULT Initialize();
	UINT loadSound(std::string strFilename);

	//Create an emitter for sound
	UINT playSound(UINT soundId, EmitterType type);
	UINT playSound3D(UINT soundId, float x, float y, float z);

	//Remove the emitter
	void stopSound(UINT emitterId);

	void handleEvent(Event& e);

	void SetListener(float EyeX, float EyeY, float EyeZ, float FrontX, float FrontY, float FrontZ, float TopX, float TopY, float TopZ);

private:	
	// array that stores the loaded sounds
	std::vector<SoundData> m_sounds;

	// array that stores all the emitters in the world
	std::vector<SoundEmitter> m_emitters;

	// the XAudio2 Interface
	IXAudio2* pXAudio2;

	// the master voice representing the audiodevice
	IXAudio2MasteringVoice* pMasteringVoice;

	// the array of all the active emitters around the camera
	std::vector<SoundEmitter> m_closeEmitters;

	// the X3DAudio Interface
	X3DAUDIO_HANDLE X3DInstance;	

	// listener struct
	X3DAUDIO_LISTENER Listener;

	WORD m_destinationChannels;
};

#endif // AUDIOSYSTEM_H