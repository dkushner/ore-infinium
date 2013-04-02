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

#include "debugsettings.h"
#include "gui.h"

#include "src/client/client.h"
#include <src/debug.h>

#include <Rocket/Core.h>
#include <Rocket/Controls.h>

#include <iostream>
#include <sstream>

#include <assert.h>

DebugSettings::DebugSettings(Client* client) : m_client(client)
{
    loadDocument();
}

DebugSettings::~DebugSettings()
{
}

void DebugSettings::ProcessEvent(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    const Rocket::Core::String& type = event.GetType();

    if (id == "sendButton") {
    } else if (type == "keydown") {
        if (event.GetParameter<int>("key_identifier", Rocket::Core::Input::KI_0) == Rocket::Core::Input::KI_RETURN) {
        }
    }
}

void DebugSettings::loadDocument()
{
    m_debugSettings = GUI::instance()->context()->LoadDocument("../client/gui/assets/chatDialog.rml");
    m_debugSettings->GetElementById("title")->SetInnerRML("Chat");

//    m_tabSet = dynamic_cast<Rocket::Controls::ElementTabSet*>(m_chat->GetElementById("tabset"));

    m_debugSettings->GetElementById("sendButton")->AddEventListener("click", this);
    m_debugSettings->GetElementById("inputLine")->AddEventListener("keydown", this);
}

void DebugSettings::show()
{
    m_debugSettings->Show();
    //focus something else, so the input line doesn't get input by default..
    m_debugSettings->GetElementById("title")->Focus();
}

void DebugSettings::close()
{
    m_debugSettings->Close();
    GUI::instance()->context()->UnloadDocument(m_debugSettings);
    //FIXME: gosh i hate this with a passion. but it's more complicated than it seems to do right. or at least i think it is.
    //also, notice i don't delete m_options. i'm seriously hoping that rocket does that. if i call delete on it, it crashsee.
    delete m_debugSettings;
}

bool DebugSettings::visible()
{
    return m_debugSettings->IsVisible();
}

Rocket::Core::ElementDocument* DebugSettings::document()
{
    return m_debugSettings;
}

