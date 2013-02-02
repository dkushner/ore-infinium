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
#include "mainmenulistener.h"

#include "../game.h"

#include <Rocket/Core.h>

#include <assert.h>

MainMenu::MainMenu(Game* game) : m_game(game)
{
    m_menu = GUI::instance()->context()->LoadDocument("../gui/assets/mainMenu.rml");
    assert(m_menu);
    m_menu->GetElementById("title")->SetInnerRML("fuck yeah, runtime text");

    m_listener = new MainMenuListener(m_game);

    m_menu->GetElementById("element1")->AddEventListener("click", m_listener);
}

MainMenu::~MainMenu()
{

}

void MainMenu::toggleShown()
{
    if (m_menu->IsVisible()) {
        m_menu->Hide();
    } else {
        m_menu->Show();
    }
}

