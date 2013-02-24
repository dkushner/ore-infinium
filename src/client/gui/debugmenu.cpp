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

#include <Rocket/Core.h>
#include <Rocket/Controls.h>

#include <iostream>
#include <sstream>

#include <assert.h>

DebugMenu::DebugMenu(Client* client) : m_client(client)
{
    loadDocument();
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
//    m_debug->GetElementById("inputLine")->AddEventListener("keydown", this);
}

void DebugMenu::update()
{
    std::stringstream ss;
    std::string str;

    ss.str("");
    ss << "FPS: " << m_client->fps() << " Frametime: " << "fucking hell";//frametime;
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
        ss << "Player Position X: " << m_client->mainPlayer()->position().x << " Y: " << m_client->mainPlayer()->position().x;
    }

    std::string playerString;

    m_debug->GetElementById("1")->SetInnerRML(str.c_str());
    m_debug->GetElementById("2")->SetInnerRML("F5 to toggle debug logging");
    m_debug->GetElementById("3")->SetInnerRML("F6 to toggle renderer logging");
    m_debug->GetElementById("4")->SetInnerRML("F7 to toggle GUI renderer debug");
    m_debug->GetElementById("5")->SetInnerRML("F8 instant multiplayer host session");
    m_debug->GetElementById("6")->SetInnerRML(connectedString.c_str());
    m_debug->GetElementById("7")->SetInnerRML(playerString.c_str());
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

