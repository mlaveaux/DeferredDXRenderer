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


#include "EngineCore.h"
#include "ConVar.h"

#include <fstream>

EngineCore::EngineCore() : IEventListener("core"), m_userWnd(&m_keybindManager)
{

}

HRESULT EngineCore::initialize(HINSTANCE hInstance, int cmdShow)
{
    HRESULT hr = S_OK;
    m_dwTimeOld = GetTickCount();
    m_timeLeft = 100;

    // initialize EventHandler and create the DebugWnd.
    if (FAILED(m_debugWnd.initWindow(hInstance, cmdShow))) {
        return E_FAIL;
    }

    if (FAILED(EventManager::Instance().initialize(&m_debugWnd))) {
        return E_FAIL;
    }

    // load the config file for keybinds and cvars. Then issue all events in the queue.
    if (FAILED(loadConfig("../user/config/settings.cfg"))) {
        return E_FAIL;
    }

    EventManager::Instance().handleEvents();

    // creates the UserWND.
    if (FAILED(m_userWnd.initWindow(hInstance, cmdShow))) {
        return E_FAIL;
    }

    ConVar m_pWindowSize("r_windowsize");

    m_userWnd.resize(m_pWindowSize.toVector());

    // initialize the DX10RenderSystem.
    m_pRenderSystem = std::unique_ptr<DX11RenderSystem>(new DX11RenderSystem());

    if (FAILED(m_pRenderSystem->initDevice(m_userWnd.getHandle()))) {
        return E_FAIL;
    }

    // initialize GUInterface.
    if (FAILED(m_gUInterface.initialize(m_pRenderSystem->getSurface()))) {
        return E_FAIL;
    }

    // initialize the AudioSystem.
    if (FAILED(m_audioSystem.Initialize())) {
        return E_FAIL;
    }

    // init state manager.
    m_stateManager.initialize(m_pRenderSystem->getSceneManager(), &m_audioSystem, &m_gUInterface);

    m_pRenderSystem->initDebug(new DebugElement(m_gUInterface.getMainElement()));

    DWORD dwTimeCurrent = GetTickCount();
    float fTime = (float)(dwTimeCurrent - m_dwTimeOld);

    EventManager::Instance().postMessage("Engine startup time: %d ms", (int)fTime);

    m_dwTimeOld = GetTickCount();

    return S_OK;
}

StateManager* EngineCore::getStateManager()
{
    return &m_stateManager;
}

DX11RenderSystem* EngineCore::getRenderSystem()
{
    return m_pRenderSystem.get();
}

GUInterface* EngineCore::getGUInterface()
{
    return &m_gUInterface;
}

// handles engine EngineCore events
void EngineCore::handleEvent(Event& e)
{
    if (strcmp(e.getID(), "command") == 0) {
        parse(e[0].toChar());
    }
    else if (strcmp(e.getID(), "cvarchange") == 0) {
        if (strcmp(e[0].toChar(), "r_windowsize") == 0)	{
            //CValue* value = e.args[0].toLong();
            //m_userWnd.resize(value->Vector);
        }
    }
    else if (strcmp(e.getID(), "quit") == 0){
        ::PostQuitMessage(0);
    }
}

void EngineCore::render(float fTime)
{
    EventManager::Instance().handleEvents();

    m_timeLeft -= fTime;
    if (m_timeLeft <= 0) {
        m_stateManager.tick();
        m_timeLeft += 100;
    }

    m_stateManager.render(fTime);

    m_pRenderSystem->beginFrame(fTime);

    m_gUInterface.render();

    m_pRenderSystem->presentFrame(fTime);
}

HRESULT EngineCore::loadConfig(const char* strFilename)
{
    std::string strLine;
    std::ifstream inFile;

    inFile.open(strFilename);

    if (!inFile) {
        EventManager::Instance().postMessage("Failed to load config: %s, settings are set to default.", strFilename);
        return S_OK;
    }

    while (getline(inFile, strLine)) {
        char* strCommand = new char[strlen(strLine.c_str()) + 1];

        ::strcpy_s(strCommand, strlen(strLine.c_str()) + 1, strLine.c_str());

        Event e = parse(strCommand);
    }

    return S_OK;
}

// parse a command into engine events and returns them.
Event EngineCore::parse(std::string strCommand)
{
    size_t index = strCommand.find_first_of(" ");
    std::string strAction = strCommand.substr(0, index);
    strCommand.erase(0, index + 1);

    if (strAction.compare("set") == 0) {
        index = strCommand.find(" ");
        std::string strVariable = strCommand.substr(0, index);

        std::string strValue = strCommand.substr(index + 1, strCommand.length());

        Event e("set", 2);
        e[0] = EventArg(strVariable);
        e[1] = EventArg(strValue);

        postEvent(e);

        return e;
    }
    else if (strAction.compare("bind") == 0) {
        index = strCommand.find(" ");
        std::string strChar = strCommand.substr(0, index);

        std::string strEvent = strCommand.substr(index + 3, strCommand.length() - index - 4);

        Event e(strEvent, 1);
        e[0] = EventArg(true);

        m_keybindManager.bind(strChar, e, KEYBIND_STAY);
    }

    return Event(strAction.c_str(), 0);
}