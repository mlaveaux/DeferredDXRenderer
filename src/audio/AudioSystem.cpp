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

#include "AudioSystem.h"

#include "main/EventManager.h"
#include "main/ConVar.h"

ConVar au_mainvolume("au_mainvolume", CValue(50), CVAR_FNONE, "");
ConVar au_channels("au_channel", CValue(32), CVAR_FNONE, "");

int LoadSound(SoundData& sound, const char* strFilename)
{
    BYTE* pbBuffer = NULL;
    DWORD arraySize;
    HRESULT hr;

    try {
        // load the file.
        hr = loadfile(&pbBuffer, arraySize, strFilename);
        if (hr != S_OK) {
            throw((DWORD)hr);
        }

        UINT Offset = 0;

        // read the wav header.
        DWORD dwBytesRead = 0;
        DWORD ChuckType = getDWordInverted(Offset, pbBuffer);
        Offset += 4;
        DWORD ChuckDataSize = getDWordInverted(Offset, pbBuffer);
        Offset += 4;
        DWORD RiffType = getDWordInverted(Offset, pbBuffer);
        Offset += 4;

        // validate the file as a .wav formatted file
        if (RiffType != 0x45564157) {
            throw(E_CORRUPTEDFILE);
        }

        DWORD FormatChunk = getDWordInverted(Offset, pbBuffer);
        Offset += 4;

        // store all the values into the pwfx.
        if (FormatChunk != 0x20746d66) {
            throw(E_CORRUPTEDFILE);
        }

        DWORD FormatSize = getDWordInverted(Offset, pbBuffer);
        Offset += 4;
        sound.pwfx.wFormatTag = getWordInverted(Offset, pbBuffer);
        Offset += 2;
        sound.pwfx.nChannels = getWordInverted(Offset, pbBuffer);
        Offset += 2;
        sound.pwfx.nSamplesPerSec = getDWordInverted(Offset, pbBuffer);
        Offset += 4;
        sound.pwfx.nAvgBytesPerSec = getDWordInverted(Offset, pbBuffer);
        Offset += 4;
        WORD BlockAlign = getWordInverted(Offset, pbBuffer);
        Offset += 2;
        sound.pwfx.wBitsPerSample = getWordInverted(Offset, pbBuffer);
        Offset += 2;
        sound.pwfx.cbSize = getWordInverted(Offset, pbBuffer);
        Offset += 2;
        sound.pwfx.nBlockAlign = sound.pwfx.nChannels * sound.pwfx.wBitsPerSample / 8;
        Offset += 2;
        sound.dwNumOfBytes = arraySize - Offset;
        sound.pbData = NULL;
        sound.pbData = (BYTE*)malloc(sound.dwNumOfBytes);

        for (DWORD i = 0; i < sound.dwNumOfBytes; ++i) {
            sound.pbData[i] = pbBuffer[i + Offset];
        }
    }
    catch (DWORD Error) {
        //File not found or other exceptions
        SAFE_DELETE_ARRAY(pbBuffer);
        SAFE_DELETE_ARRAY(sound.pbData);

        EventManager::Instance().postMessage("Loading %s failed, exception: %d", strFilename, (DWORD)Error);

        return -1;
    }

    SAFE_DELETE_ARRAY(pbBuffer);

    return 0;
}

AudioSystem::AudioSystem() : IEventListener("audiosystem")
{
    m_sounds.push_back(SoundData()); // Create an empty sound.
}

HRESULT AudioSystem::Initialize()
{
    HRESULT hr;

    // Initialize XAudio2
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    UINT32 flags = 0;

#ifdef _DEBUG
    //flags |= XAUDIO2_DEBUG_ENGINE;
#endif

    hr = XAudio2Create(&pXAudio2, flags, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) {
        return EventManager::Instance().postException(hr, "XAudio2Create() failed");
    }

    //Create the master voice
    if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL))) {
        SAFE_RELEASE(pXAudio2);
        CoUninitialize();
        return EventManager::Instance().postException(hr, "CreateMasteringVoice() failed");
    }
    
    m_closeEmitters.reserve(32);
    m_destinationChannels = 5;

    return S_OK;
}

UINT AudioSystem::loadSound(std::string strFilename)
{
    //Make the correct relative path
    std::string strPath = "..\\data\\sounds\\";
    strPath.append(strFilename);

    //Make space for a new sound
    SoundData tempSound;

    //Load the sound
    if (LoadSound(tempSound, strPath.c_str()) != 0) {
        return 0;
    }

    m_sounds.push_back(tempSound);

    UINT index = (UINT)m_sounds.size() - 1;
    assert(index == m_sounds.size() - 1); // Check if value was truncated.

    return index;
}

void AudioSystem::handleEvent(Event& e)
{

}

void AudioSystem::SetListener(float EyeX, float EyeY, float EyeZ, float FrontX, float FrontY, float FrontZ, float TopX, float TopY, float TopZ)
{
    Listener.pCone = NULL;

    Listener.Position.x = EyeX;
    Listener.Position.y = EyeY;
    Listener.Position.z = EyeZ;

    Listener.OrientFront.x = FrontX;
    Listener.OrientFront.y = FrontY;
    Listener.OrientFront.z = FrontZ;

    Listener.OrientTop.x = TopX;
    Listener.OrientTop.y = TopY;
    Listener.OrientTop.z = TopZ;


    for (int i = 0; i < 32; ++i) {
        if (m_closeEmitters[i].Active) {
            X3DAudioCalculate(X3DInstance, &Listener, &m_closeEmitters[i].X3DEmitter, X3DAUDIO_CALCULATE_MATRIX, &m_closeEmitters[i].X3DDSPSettings);

            if (m_closeEmitters[i].X3DEmitter.ChannelCount > 0) {
                m_closeEmitters[i].pSourceVoice->SetOutputMatrix(NULL,
                    m_closeEmitters[i].X3DEmitter.ChannelCount,
                    m_closeEmitters[i].X3DDSPSettings.DstChannelCount,
                    m_closeEmitters[i].X3DDSPSettings.pMatrixCoefficients,
                    0);
            }
        }
    }
}

UINT AudioSystem::playSound(UINT soundId, EmitterType type)
{
    HRESULT hr;

    SoundData& sound = m_sounds[soundId];
    if (sound.pbData == NULL) {
        return 0;
    }

    //Initialize the source voice
    int i = 0;

    for (i = 0; i < 32; ++i) {
        if (!m_closeEmitters[i].Active) {
            if (m_closeEmitters[i].pSourceVoice != NULL) {
                m_closeEmitters[i].pSourceVoice->DestroyVoice();
                m_closeEmitters[i].pSourceVoice = NULL;
            }

            if (FAILED(hr = pXAudio2->CreateSourceVoice(&m_closeEmitters[i].pSourceVoice, &sound.pwfx))) {
                return 0;
            }

            // Submit the wave sample data using an XAUDIO2_BUFFER structure
            XAUDIO2_BUFFER buffer = { 0 };
            buffer.pAudioData = sound.pbData;
            buffer.AudioBytes = sound.dwNumOfBytes;
            buffer.Flags = XAUDIO2_END_OF_STREAM;
            buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

            if (FAILED(hr = m_closeEmitters[i].pSourceVoice->SubmitSourceBuffer(&buffer))) {
                m_closeEmitters[i].pSourceVoice->DestroyVoice();
                return 0;
            }

            //Start playing the sound
            m_closeEmitters[i].pSourceVoice->Start(0, XAUDIO2_COMMIT_NOW);

            m_closeEmitters[i].Active = true;

            return i;
        }
    }

    return 0;
}

UINT AudioSystem::playSound3D(UINT soundId, float x, float y, float z)
{
    //HRESULT hr;

    UINT i = playSound(soundId, EMITTER_REPEAT);
    if (i == 0) {
        return 0;
    }

    SoundData& sound = m_sounds[soundId];

    SoundEmitter& emitter = m_closeEmitters[i];
    //	return;

    emitter.X3DEmitter.Position.x = x;
    emitter.X3DEmitter.Position.y = y;
    emitter.X3DEmitter.Position.z = z;

    emitter.X3DEmitter.ChannelCount = sound.pwfx.nChannels;
    emitter.X3DEmitter.CurveDistanceScaler = 1.0f;

    SAFE_DELETE_ARRAY(emitter.X3DEmitter.pChannelAzimuths);

    emitter.X3DEmitter.pChannelAzimuths = new float[emitter.X3DEmitter.ChannelCount];

    SAFE_DELETE_ARRAY(emitter.X3DDSPSettings.pMatrixCoefficients);

    emitter.X3DDSPSettings.pMatrixCoefficients = new float[emitter.X3DEmitter.ChannelCount * m_destinationChannels];
    emitter.X3DDSPSettings.pDelayTimes = NULL;

    emitter.X3DDSPSettings.DstChannelCount = m_destinationChannels;
    emitter.X3DDSPSettings.SrcChannelCount = emitter.X3DEmitter.ChannelCount;

    return i;
}

void AudioSystem::stopSound(UINT emitterId)
{

}