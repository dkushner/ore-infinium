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

#ifndef QUICKBARMENU_H
#define QUICKBARMENU_H

#include <vector>
#include <string>

#include <Rocket/Core.h>
#include <SDL_events.h>

namespace Rocket
{
    namespace Core
    {
        class ElementDocument;
    }
}

class Client;

class QuickBarMenu : public Rocket::Core::EventListener
{
public:
    QuickBarMenu(Client* client);
    ~QuickBarMenu();

    void ProcessEvent(Rocket::Core::Event& event);

    void loadDocument();
    Rocket::Core::ElementDocument* document();

    bool visible();

    void show();

    void handleEvent(const SDL_Event& event);

    /**
     * @p index 0-based index of the slot and data stored therein
     */
    void setSlot(uint8_t index, std::string str);

private:
    void selectSlot(uint8_t index);

    std::vector<std::string> m_items;

    Client* m_client = nullptr;

    Rocket::Core::ElementDocument* m_menu = nullptr;
};

#endif
