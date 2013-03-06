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

#include "quickbarmenu.h"

#include "src/client/client.h"
#include "src/client/gui/gui.h"

#include <src/debug.h>

#include <Rocket/Core.h>
#include <Rocket/Controls.h>

QuickBarMenu::QuickBarMenu(Client* client)
:   m_client(client)
{
    m_items.push_back("test1");
    m_items.push_back("test2");
    m_items.push_back("test3");
    m_items.push_back("test4");

    loadDocument();
}

QuickBarMenu::~QuickBarMenu()
{

}

void QuickBarMenu::ProcessEvent(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    const Rocket::Core::String& type = event.GetType();

    if (id == "sendButton") {
    } else if (type == "keydown") {
        if (event.GetParameter<int>("key_identifier", Rocket::Core::Input::KI_0) == Rocket::Core::Input::KI_RETURN) {
        }
    }
}

void QuickBarMenu::loadDocument()
{
    m_chat = GUI::instance()->context()->LoadDocument("../client/gui/assets/chatDialog.rml");
    m_chat->GetElementById("title")->SetInnerRML("Chat");

//    m_tabSet = dynamic_cast<Rocket::Controls::ElementTabSet*>(m_chat->GetElementById("tabset"));

    m_chat->GetElementById("sendButton")->AddEventListener("click", this);
    m_chat->GetElementById("inputLine")->AddEventListener("keydown", this);
}

bool QuickBarMenu::visible()
{
    return m_chat->IsVisible();
}

Rocket::Core::ElementDocument* QuickBarMenu::document()
{
    return m_chat;
}

