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

#include "mainmenu.h"
#include "gui.h"

#include "src/client/client.h"

#include <Rocket/Core.h>

#include <iostream>
#include <sstream>

#include <assert.h>

MainMenu::MainMenu(Client* client) : m_client(client)
{
    m_menu = GUI::instance()->context()->LoadDocument("../gui/assets/mainMenu.rml");
    assert(m_menu);

    m_menu->GetElementById("singleplayer")->AddEventListener("click", this);
    m_menu->GetElementById("multiplayer")->AddEventListener("click", this);
    m_menu->GetElementById("options")->AddEventListener("click", this);
    m_menu->GetElementById("quit")->AddEventListener("click", this);
}

MainMenu::~MainMenu()
{

}

void MainMenu::ProcessEvent(Rocket::Core::Event& event)
{
    std::cout << "mainmenu Processing element: " << event.GetCurrentElement()->GetId().CString() << " type: " << event.GetType().CString() << '\n';

    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();

    if (id == "singleplayer") {
        std::stringstream ss;
        ss << "Player";
        ss << rand();
        m_client->startSinglePlayer(ss.str());
    } else if (id == "multiplayer") {
        std::stringstream ss;
        ss << "Player";
        ss << rand();
        m_client->startMultiplayerClientConnection(ss.str(), "127.0.0.1", 44543);
//        m_client->startMultiplayerHostConnection("Shaun");
    } else if (id == "options") {
        if (!m_optionsDialog) {
            m_optionsDialog = new OptionsDialog(m_client, this);
        }

        m_optionsDialog->show();
    } else if (id == "quit") {
        m_client->shutdown();
    }
}

void MainMenu::toggleShown()
{
    if (m_menu->IsVisible()) {
        GUI::instance()->removeInputDemand();
        m_menu->Hide();

        if (m_optionsDialog) {
            m_optionsDialog->close();
        }
    } else {
        GUI::instance()->addInputDemand();
        m_menu->Show();
    }
}

void MainMenu::optionsClosedCallback()
{
    delete m_optionsDialog;
    m_optionsDialog = 0;
}


