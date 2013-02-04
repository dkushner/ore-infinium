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

#include "../game.h"

#include <Rocket/Core.h>

#include <iostream>

#include <assert.h>

MainMenu::MainMenu(Game* game) : m_game(game)
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

    } else if (id == "options") {
        if (!m_optionsDialog) {
            m_optionsDialog = new OptionsDialog(m_game, this);
        }

        m_optionsDialog->show();
    } else if (id == "quit") {
        m_game->shutdown();
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


