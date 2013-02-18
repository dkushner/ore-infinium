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

#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <Rocket/Core.h>

namespace Rocket {
    namespace Core {
        class ElementDocument;
    }
    namespace Controls{
        class ElementTabSet;
    }
}

class Client;
class MainMenu;

class ChatDialog : public Rocket::Core::EventListener
{
public:
    ChatDialog(Client* client, MainMenu* parent);
    ~ChatDialog();

    /// reimplemented from Rocket::Core::EventListener
    virtual void ProcessEvent(Rocket::Core::Event& event);

    void show();
    void close();
    bool visible();

    void clearChatHistory();

    void addChatLine(const std::string& playerName, const std::string& message);

    Rocket::Core::ElementDocument* document();

    static constexpr int maximumChatCharCount = 256;

private:
    void loadDocument();
    void reloadChatHistory();
    void consumeInputLine();

    void replaceAll(std::string& str, const std::string& from, const std::string& to);

private:
    Client* m_client = nullptr;
    MainMenu* m_parent = nullptr;

    Rocket::Core::ElementDocument* m_chat = nullptr;
    Rocket::Controls::ElementTabSet* m_tabSet = nullptr;

    std::deque<std::string> m_chatHistory;
};

#endif
