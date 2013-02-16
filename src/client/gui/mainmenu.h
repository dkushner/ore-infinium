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

#ifndef MAINMENU_H
#define MAINMENU_H

#include <Rocket/Core.h>
#include "optionsdialog.h"

namespace Rocket {
    namespace Core {
        class ElementDocument;
    }
}

class Client;

class MainMenu : public Rocket::Core::EventListener
{
public:
    MainMenu(Client* client);
    ~MainMenu();

    /// reimplemented from Rocket::Core::EventListener
    virtual void ProcessEvent(Rocket::Core::Event& event);

    void showEscapeMenu();
    void hideEscapeMenu();
    bool escapeMenuVisible();

    void hideMainMenu();
    void showMainMenu();

    void showOptionsDialog();
    void optionsClosedCallback();

private:
    void processEscapeMenu(Rocket::Core::Event& event);
    void processMainMenu(Rocket::Core::Event& event);
    void processSingleplayer(Rocket::Core::Event& event);
    void processSingleplayerCreate(Rocket::Core::Event& event);
    void processSingleplayerLoad(Rocket::Core::Event& event);
    void processMultiplayer(Rocket::Core::Event& event);
    void processMultiplayerHost(Rocket::Core::Event& event);
    void processMultiplayerJoin(Rocket::Core::Event& event);

    /// hides all submenus, including multiplayer, sp, if available, as we're initiating the game now
    void hideSubmenus();

private:
    Client* m_client = nullptr;
    OptionsDialog* m_optionsDialog = nullptr;

    Rocket::Core::ElementDocument* m_menu = nullptr;
    Rocket::Core::ElementDocument* m_escapeMenu = nullptr;
    Rocket::Core::ElementDocument* m_mainMenuSingleplayer = nullptr;
    Rocket::Core::ElementDocument* m_mainMenuSingleplayerCreate = nullptr;
    Rocket::Core::ElementDocument* m_mainMenuSingleplayerLoad = nullptr;
    Rocket::Core::ElementDocument* m_mainMenuMultiplayer = nullptr;
    Rocket::Core::ElementDocument* m_mainMenuMultiplayerHost = nullptr;
    Rocket::Core::ElementDocument* m_mainMenuMultiplayerJoin = nullptr;
};

#endif
