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
}

void DebugMenu::setCollapsed(bool collapsed)
{
    m_collapsed = collapsed;

    int height = 135;
    int width = 34;

    if (!collapsed) {
        height = Settings::instance()->screenResolutionHeight;
        width = 100;
    }

    Rocket::Core::Colourb shit(255, 0, 0, 55);
    m_debug->SetProperty("background-color", Rocket::Core::Property(shit, Rocket::Core::Property::COLOUR));
    m_debug->SetProperty("height", Rocket::Core::Property(height, Rocket::Core::Property::PX));
    m_debug->SetProperty("width", Rocket::Core::Property(width, Rocket::Core::Property::PERCENT));
}

void DebugMenu::update(double frameTime)
{
    std::stringstream ss;
    std::string fpsString;

    static double previousFrameTime = frameTime;
    static int limiter = 0;
    ++limiter;

    //FIXME: pretty hacky, eh? But it's just to limit update time for framerate so you can actually *see* it
    if (limiter > 10) {
        previousFrameTime = frameTime;
        limiter = 0;

        ss.str("");
        ss << "FPS: " << (1000.0 / previousFrameTime) << " Frametime: " << (previousFrameTime / 1000.0);
        fpsString = ss.str();

        m_debug->GetElementById("1")->SetInnerRML(fpsString.c_str());
    }

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
        ss.str("");

        m_debug->GetElementById("5")->SetInnerRML(playerString.c_str());
    }

    ss << "Physics Body Count: " << m_physicsWorldBodyCount;

    std::string physicsBodyCount = ss.str();
    ss.str("");

    m_debug->GetElementById("6")->SetInnerRML(physicsBodyCount.c_str());

    m_debug->GetElementById("4")->SetInnerRML(connectedString.c_str());
    m_debug->GetElementById("2")->SetInnerRML("F8 instant multiplayer host session");
    m_debug->GetElementById("3")->SetInnerRML("F11 to show/hide debug settings menu");
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

