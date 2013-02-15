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

#include "src/client/client.h"
#include <src/debug.h>

#include <Rocket/Core.h>
#include <Rocket/Controls.h>

#include <iostream>
#include <sstream>

#include <assert.h>

ChatDialog::ChatDialog(Client* client, MainMenu* parent) : m_client(client), m_parent(parent)
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
    const Rocket::Core::String& type = event.GetType();

    if (id == "sendButton") {
        consumeInputLine();
    } else if (type == "keydown") {
        if (event.GetParameter<int>("key_identifier", Rocket::Core::Input::KI_0) == Rocket::Core::Input::KI_RETURN) {
            consumeInputLine();
        }
    }
}

void ChatDialog::loadDocument()
{
    m_chat = GUI::instance()->context()->LoadDocument("../client/gui/assets/chatDialog.rml");
    m_chat->GetElementById("title")->SetInnerRML("Chat");

    m_tabSet = dynamic_cast<Rocket::Controls::ElementTabSet*>(m_chat->GetElementById("tabset"));

    m_chat->GetElementById("sendButton")->AddEventListener("click", this);
    m_chat->GetElementById("inputLine")->AddEventListener("keydown", this);
    reloadChatHistory();
}

void ChatDialog::reloadChatHistory()
{
    std::stringstream ss;

    for (auto& currentLine : m_chatHistory) {
        ss << currentLine;
    }
    m_tabSet->GetElementById("panel1")->SetInnerRML(ss.str().c_str());
    //somewhat big numbah to get it to want to scroll to the bottom all the time
    m_tabSet->GetElementById("panel1")->SetScrollTop(20500.0f);
}

void ChatDialog::replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty()) {
        return;
    }

    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void ChatDialog::addChatLine(const std::string& message)
{
    if (message != "") {
        std::string formattedMessage = message;

        std::string finalString;
        std::stringstream ss;

        Debug::assertf(message.length() < maximumChatCharCount, "FATAL, ChatDialog::addChatLine was given an input string larger than the  maximum char count, for buffer overflow prevention. This is a BUG, likely in the networking stack.");

        ss << "playername: ";

        // for sanitizing, so clients can't inject arbitrary html(!), which would mean they could
        // do all kinds of crazy shit.
        replaceAll(formattedMessage, "<", "&lt;");
        replaceAll(formattedMessage, ">", "&gt;");

        ss << formattedMessage;

        ss << "<br/>";
        finalString = ss.str();

        m_chatHistory.insert(m_chatHistory.end(), finalString);

        reloadChatHistory();
    }
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

