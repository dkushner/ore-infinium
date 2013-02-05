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

#include "chatdialog.h"
#include "mainmenu.h"
#include "gui.h"

#include "../game.h"

#include <Rocket/Core.h>
#include <Rocket/Controls.h>

#include <iostream>
#include <sstream>

#include <assert.h>

ChatDialog::ChatDialog(Game* game, MainMenu* parent) : m_game(game), m_parent(parent)
{
    loadDocument();
}

ChatDialog::~ChatDialog()
{
}

void ChatDialog::ProcessEvent(Rocket::Core::Event& event)
{
    std::cout << "Options Processing element: " << event.GetCurrentElement()->GetId().CString() << " type: " << event.GetType().CString() << '\n';
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();

    if (id == "sendButton") {
        consumeInputLine();
    }
}

void ChatDialog::loadDocument()
{
    m_chat = GUI::instance()->context()->LoadDocument("../gui/assets/chatDialog.rml");
    m_chat->GetElementById("title")->SetInnerRML("fuck yeah, runtime chat");

    m_tabSet = dynamic_cast<Rocket::Controls::ElementTabSet*>(m_chat->GetElementById("tabset"));

    m_chat->GetElementById("sendButton")->AddEventListener("click", this);
    reloadChatHistory();
}

void ChatDialog::reloadChatHistory()
{
    std::stringstream ss;

    for (auto& currentLine : m_chatHistory) {
        ss << currentLine;
    }

    m_tabSet->GetElementById("panel1")->SetInnerRML(ss.str().c_str());
}

void ChatDialog::addChatLine(const std::string& message)
{
    std::string finalString;
    std::stringstream ss;

    ss << message;
    ss << "<br/>";
    finalString = ss.str();

    m_chatHistory.insert(m_chatHistory.end(), finalString);

    reloadChatHistory();
}

void ChatDialog::consumeInputLine()
{
    Rocket::Controls::ElementFormControlInput* inputLine = dynamic_cast<Rocket::Controls::ElementFormControlInput*>(m_chat->GetElementById("inputLine"));
    const std::string& input = inputLine->GetAttribute<Rocket::Core::String>("value", "").CString();

    addChatLine(input);
    inputLine->SetAttribute<Rocket::Core::String>("value", "");

    reloadChatHistory();
}

void ChatDialog::clearChatHistory()
{
    m_chatHistory.clear();
    reloadChatHistory();
}

void ChatDialog::show()
{
    m_chat->Show();
}

void ChatDialog::close()
{
    m_chat->Close();
    GUI::instance()->context()->UnloadDocument(m_chat);
    //FIXME: gosh i hate this with a passion. but it's more complicated than it seems to do right. or at least i think it is.
    //also, notice i don't delete m_options. i'm seriously hoping that rocket does that. if i call delete on it, it crashsee.
    delete m_chat;
}

bool ChatDialog::visible()
{
    return m_chat->IsVisible();
}

Rocket::Core::ElementDocument* ChatDialog::document()
{
    return m_chat;
}

