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
#include "src/settings/settings.h"

#include "src/debug.h"

#include <Rocket/Core.h>
#include <Rocket/Controls.h>

#include <iostream>
#include <sstream>

#include <random>

#include <assert.h>

MainMenu::MainMenu(Client* client) : m_client(client)
{

    m_menu = GUI::instance()->context()->LoadDocument("../client/gui/assets/mainMenu.rml");
    assert(m_menu);

//    m_menu->SetProperty("height", Rocket::Core::Property(Settings::instance()->screenResolutionHeight ,Rocket::Core::Property::PX));
//    m_menu->SetProperty("width", Rocket::Core::Property(Settings::instance()->screenResolutionWidth,Rocket::Core::Property::PX));
    m_menu->GetElementById("content")->SetProperty("padding-top", Rocket::Core::Property(static_cast<int>(Settings::instance()->screenResolutionHeight * 0.5), Rocket::Core::Property::PX));

    m_menu->GetElementById("singleplayer")->AddEventListener("click", this);
    m_menu->GetElementById("multiplayer")->AddEventListener("click", this);
    m_menu->GetElementById("options")->AddEventListener("click", this);
    m_menu->GetElementById("quit")->AddEventListener("click", this);

    m_escapeMenu = GUI::instance()->context()->LoadDocument("../client/gui/assets/escapeMenu.rml");
    assert(m_escapeMenu);

    m_escapeMenu->SetProperty("height", Rocket::Core::Property(Settings::instance()->screenResolutionHeight , Rocket::Core::Property::PX));
    m_escapeMenu->SetProperty("width", Rocket::Core::Property(Settings::instance()->screenResolutionWidth, Rocket::Core::Property::PX));
    m_escapeMenu->GetElementById("content")->SetProperty("padding-top", Rocket::Core::Property(static_cast<int>(Settings::instance()->screenResolutionHeight * 0.5), Rocket::Core::Property::PX));
    m_escapeMenu->GetElementById("resume")->AddEventListener("click", this);
    m_escapeMenu->GetElementById("disconnect")->AddEventListener("click", this);
    m_escapeMenu->GetElementById("options")->AddEventListener("click", this);
    m_escapeMenu->GetElementById("quit")->AddEventListener("click", this);

    m_mainMenuSingleplayer = GUI::instance()->context()->LoadDocument("../client/gui/assets/mainMenuSingleplayer.rml");
    assert(m_mainMenuSingleplayer);
    m_mainMenuSingleplayer->GetElementById("create")->AddEventListener("click", this);
    m_mainMenuSingleplayer->GetElementById("load")->AddEventListener("click", this);
    m_mainMenuSingleplayer->GetElementById("back")->AddEventListener("click", this);

    m_mainMenuSingleplayerCreate = GUI::instance()->context()->LoadDocument("../client/gui/assets/mainMenuSingleplayerCreate.rml");
    m_mainMenuSingleplayerCreate->GetElementById("back")->AddEventListener("click", this);
    m_mainMenuSingleplayerCreate->GetElementById("start")->AddEventListener("click", this);

    m_mainMenuSingleplayerLoad = GUI::instance()->context()->LoadDocument("../client/gui/assets/mainMenuSingleplayerLoad.rml");
    m_mainMenuSingleplayerLoad->GetElementById("back")->AddEventListener("click", this);

    m_mainMenuMultiplayer = GUI::instance()->context()->LoadDocument("../client/gui/assets/mainMenuMultiplayer.rml");
    m_mainMenuMultiplayer->GetElementById("back")->AddEventListener("click", this);
    m_mainMenuMultiplayer->GetElementById("host")->AddEventListener("click", this);
    m_mainMenuMultiplayer->GetElementById("join")->AddEventListener("click", this);

    m_mainMenuMultiplayerHost = GUI::instance()->context()->LoadDocument("../client/gui/assets/mainMenuMultiplayerHost.rml");
    m_mainMenuMultiplayerHost->GetElementById("back")->AddEventListener("click", this);
    m_mainMenuMultiplayerHost->GetElementById("host")->AddEventListener("click", this);

    m_mainMenuMultiplayerJoin = GUI::instance()->context()->LoadDocument("../client/gui/assets/mainMenuMultiplayerJoin.rml");
    m_mainMenuMultiplayerJoin->GetElementById("back")->AddEventListener("click", this);
    m_mainMenuMultiplayerJoin->GetElementById("join")->AddEventListener("click", this);
}

MainMenu::~MainMenu()
{

}

void MainMenu::ProcessEvent(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();

    //process submenus first since those are on top, obviously..
    if (m_mainMenuSingleplayerCreate->IsVisible()) {
        processSingleplayerCreate(event);
        return;
    } else if (m_mainMenuSingleplayerLoad->IsVisible()) {
        processSingleplayerLoad(event);
        return;
    }

    if (m_mainMenuSingleplayer->IsVisible()) {
        processSingleplayer(event);
        return;
    }

    if (m_mainMenuMultiplayerHost->IsVisible()) {
        processMultiplayerHost(event);
        return;
    }

    if (m_mainMenuMultiplayerJoin->IsVisible()) {
        processMultiplayerJoin(event);
        return;
    }

    // process parent/non-submenus
    if (m_mainMenuMultiplayer->IsVisible()) {
        processMultiplayer(event);
        return;
    }

    if (m_escapeMenu->IsVisible()) {
        //user pressed escape, aka is in game with a connection (either SP or MP)
        processEscapeMenu(event);
        return;
    }

    if (m_menu->IsVisible()) {
        processMainMenu(event);
        return;
    }
}

void MainMenu::processEscapeMenu(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();

    if (id == "resume") {
        hideEscapeMenu();
    } else if (id == "disconnect") {
        m_client->disconnect();
    } else if (id == "options") {
        showOptionsDialog();
    } else if (id == "quit") {
        //FIXME: prompt dialog to disconnect, as we clearly are connected.
        m_client->shutdown();
    }
}

void MainMenu::processMainMenu(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    if (id == "singleplayer") {
        m_mainMenuSingleplayer->Show();
    } else if (id == "multiplayer") {
        m_mainMenuMultiplayer->Show();
    } else if (id == "options") {
        showOptionsDialog();
    } else if (id == "quit") {
        // no prompt needed for save, no active connection.
        m_client->shutdown();
    }
}

void MainMenu::processSingleplayer(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    //just at the singleplayer sub menu
    if (id == "create") {
        // FIXME: populate the singleplayer create with values from settings
        Rocket::Core::Element* playerNameInput = m_mainMenuSingleplayerCreate->GetElementById("playerName");
        //HACK: pick a random useless name
        std::stringstream ss;
        ss << "Player";
        std::random_device device;
        std::mt19937 rand(device());
        std::uniform_int_distribution<> distribution(0, INT_MAX);

        ss << distribution(rand);

        playerNameInput->SetAttribute("value", ss.str().c_str());

        m_mainMenuSingleplayerCreate->Show();
    } else if (id == "load") {
        m_mainMenuSingleplayerLoad->Show();
    } else if (id == "back") {
        m_mainMenuSingleplayer->Hide();
    }
}

void MainMenu::processSingleplayerCreate(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    if (id == "back") {
        m_mainMenuSingleplayerCreate->Hide();
    } else if (id == "start") {
        Rocket::Core::Element* playerNameInput = m_mainMenuSingleplayerCreate->GetElementById("playerName");
        Rocket::Core::String playerName = playerNameInput->GetAttribute("value")->Get<Rocket::Core::String>();
        hideSubmenus();
        m_client->startSinglePlayer(playerName.CString());
    }
}

void MainMenu::processSingleplayerLoad(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    if (id == "back") {
        m_mainMenuSingleplayerLoad->Hide();
    }
}

void MainMenu::processMultiplayer(Rocket::Core::Event& event)
{
    std::random_device device;
    std::mt19937 rand(device());
    std::uniform_int_distribution<> distribution(0, INT_MAX);

    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    if (id == "back") {
        m_mainMenuMultiplayer->Hide();
    } else if (id == "host") {
        Rocket::Core::Element* playerNameInput = m_mainMenuMultiplayerHost->GetElementById("playerName");
        //HACK: pick a random useless name
        std::stringstream ss;
        ss << "Player";
        ss << distribution(rand);

        playerNameInput->SetAttribute("value", ss.str().c_str());

        m_mainMenuMultiplayerHost->Show();
    } else if (id == "join") {
        Rocket::Core::Element* playerNameInput = m_mainMenuMultiplayerJoin->GetElementById("playerName");
        //HACK: pick a random useless name
        std::stringstream ss;
        ss << "Player";
        ss << distribution(rand);

        playerNameInput->SetAttribute("value", ss.str().c_str());

        m_mainMenuMultiplayerJoin->Show();
    }
}

void MainMenu::processMultiplayerHost(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    if (id == "back") {
        m_mainMenuMultiplayerHost->Hide();
    } if (id == "host") {
        Rocket::Core::Element* playerNameInput = m_mainMenuMultiplayerHost->GetElementById("playerName");
        Rocket::Core::String playerName = playerNameInput->GetAttribute("value")->Get<Rocket::Core::String>();
        Rocket::Core::Element* portInput = m_mainMenuMultiplayerHost->GetElementById("port");
        Rocket::Core::String port = portInput->GetAttribute("value")->Get<Rocket::Core::String>();

        hideSubmenus();

        m_client->startMultiplayerHost(playerName.CString(), atoi(port.CString()));
    }
}

void MainMenu::processMultiplayerJoin(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    if (id == "back") {
        m_mainMenuMultiplayerJoin->Hide();
    } else if (id == "join") {
        Rocket::Core::Element* playerNameInput = m_mainMenuMultiplayerJoin->GetElementById("playerName");
        Rocket::Core::String playerName = playerNameInput->GetAttribute("value")->Get<Rocket::Core::String>();
        Rocket::Core::Element* ipInput = m_mainMenuMultiplayerJoin->GetElementById("ip");
        Rocket::Core::String ip = ipInput->GetAttribute("value")->Get<Rocket::Core::String>();
        Rocket::Core::Element* portInput = m_mainMenuMultiplayerJoin->GetElementById("port");
        Rocket::Core::String port = portInput->GetAttribute("value")->Get<Rocket::Core::String>();

        Debug::log(Debug::Area::NetworkClientInitialArea) << "joining multiplayer session with player: " << playerName.CString() << " ip: " << ip.CString() << " port: " << atoi(port.CString());

        if (m_client->startMultiplayerClientConnection(playerName.CString(), ip.CString(), atoi(port.CString()))) {
            hideSubmenus();
        } else {
            //failure
        }
    }
}

void MainMenu::hideSubmenus()
{
    m_mainMenuSingleplayer->Hide();
    m_mainMenuSingleplayerCreate->Hide();
    m_mainMenuSingleplayerLoad->Hide();
    m_mainMenuMultiplayerHost->Hide();
    m_mainMenuMultiplayerJoin->Hide();
    m_mainMenuMultiplayer->Hide();
}

void MainMenu::showOptionsDialog()
{
    if (!m_optionsDialog) {
        m_optionsDialog = new OptionsDialog(m_client, this);
    }

    m_optionsDialog->show();
}

bool MainMenu::escapeMenuVisible()
{
    m_escapeMenu->IsVisible();
}

void MainMenu::showEscapeMenu()
{
    if (!m_escapeMenu->IsVisible()) {
        GUI::instance()->addInputDemand();
        m_escapeMenu->Show();
    }
}

void MainMenu::hideEscapeMenu()
{
    if (m_escapeMenu->IsVisible()) {
        GUI::instance()->removeInputDemand();
        m_escapeMenu->Hide();
        if (m_optionsDialog) {
            m_optionsDialog->close();
        }
    }
}

//input demand not needed since you can't access the main menu in game, only the escape menu
void MainMenu::showMainMenu()
{
    m_menu->Show();
}

void MainMenu::hideMainMenu()
{
    m_menu->Hide();
}

void MainMenu::optionsClosedCallback()
{
    delete m_optionsDialog;
    m_optionsDialog = 0;
}
