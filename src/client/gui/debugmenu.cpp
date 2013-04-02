/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU General Public License as           *
 *   published by the Free Software Foundation; either version 2 of           *
 *   the License, or (at your option) any later version.                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/

#include "debugmenu.h"
#include "mainmenu.h"
#include "gui.h"

#include "src/client/client.h"
#include <src/debug.h>
#include <src/player.h>
#include <src/settings/settings.h>

#include <Rocket/Core.h>
#include <Rocket/Controls.h>

#include <iostream>
#include <sstream>

#include <assert.h>

DebugMenu::DebugMenu(Client* client) : m_client(client)
{
    loadDocument();
    setCollapsed(true);
}

DebugMenu::~DebugMenu()
{
}

void DebugMenu::ProcessEvent(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    const Rocket::Core::String& type = event.GetType();

    if (id == "sendButton") {
    } else if (type == "keydown") {
        if (event.GetParameter<int>("key_identifier", Rocket::Core::Input::KI_0) == Rocket::Core::Input::KI_RETURN) {
        }
    }
}

void DebugMenu::loadDocument()
{
    m_debug = GUI::instance()->context()->LoadDocument("../client/gui/assets/debugMenu.rml");

//    m_debug->GetElementById("sendButton")->AddEventListener("click", this);
}

void DebugMenu::setCollapsed(bool collapsed)
{
    m_collapsed = collapsed;

    int height = 35;
    int width = 20;

    if (!collapsed) {
        height = Settings::instance()->screenResolutionHeight;
        width = 100;
    }

    Rocket::Core::Colourb shit(255, 0, 0, 155);
    m_debug->SetProperty("background-color", Rocket::Core::Property(shit, Rocket::Core::Property::COLOUR));
    m_debug->SetProperty("height", Rocket::Core::Property(height, Rocket::Core::Property::PX));
    m_debug->SetProperty("width", Rocket::Core::Property(width, Rocket::Core::Property::PERCENT));
}

void DebugMenu::update(double frameTime)
{
    std::stringstream ss;
    std::string str;

    ss.str("");
    ss << "FPS: " << (1000.0 / frameTime) << " Frametime: " << (frameTime / 1000.0);
    str = ss.str();

    ss.str("");
    ss << "Client Connection Status: ";

    if (m_client->connected()) {
        ss << "Connected!";
    } else {
        ss << "Disconnected";
    }

    if (m_client->hosting()) {
        ss << " | Hosting Server Mode";
    } else {
        ss << " | Client Mode";
    }

    std::string connectedString = ss.str();

    ss.str("");

    if (m_client->mainPlayer()) {
        std::string playerString;
        ss << "Player Position X: " << m_client->mainPlayer()->position().x << " Y: " << m_client->mainPlayer()->position().y;
        playerString = ss.str();

        m_debug->GetElementById("8")->SetInnerRML(playerString.c_str());
    }

    ss.str("");
    ss << "F5 to toggle debug logging: ";

    if (Settings::instance()->debugOutput) {
        ss << "ENABLED";
    } else {
        ss << "disabled";
    }

    std::string debugLoggingString = ss.str();

    ss.str("");
    ss << "F6 to toggle renderer logging: ";

    if (Settings::instance()->debugRendererOutput) {
        ss << "ENABLED";
    } else {
        ss << "disabled";
    }

    std::string debugRendererOutputString = ss.str();

    ss.str("");
    ss << "F7 to toggle GUI renderer debug: ";
    if (Settings::instance()->debugGUIRenderingEnabled) {
        ss << "ENABLED";
    } else {
        ss << "disabled";
    }

    std::string debugGUIRenderer = ss.str();

    m_debug->GetElementById("1")->SetInnerRML(str.c_str());
    m_debug->GetElementById("2")->SetInnerRML(debugLoggingString.c_str());
    m_debug->GetElementById("3")->SetInnerRML(debugRendererOutputString.c_str());
    m_debug->GetElementById("4")->SetInnerRML(debugGUIRenderer.c_str());
    m_debug->GetElementById("5")->SetInnerRML("F8 instant multiplayer host session");
    m_debug->GetElementById("6")->SetInnerRML(connectedString.c_str());
    m_debug->GetElementById("7")->SetInnerRML("F9 to toggle light rendering pass");
    m_debug->GetElementById("8")->SetInnerRML("F10 to toggle tile rendering pass");
    m_debug->GetElementById("9")->SetInnerRML("F11 to show/hide debug settings menu");
    // 8 is up top, player pos
}

void DebugMenu::show()
{
    m_debug->Show();
}

void DebugMenu::close()
{
    m_debug->Close();
    GUI::instance()->context()->UnloadDocument(m_debug);
    delete m_debug;
}

bool DebugMenu::visible()
{
    return m_debug->IsVisible();
}

Rocket::Core::ElementDocument* DebugMenu::document()
{
    return m_debug;
}

