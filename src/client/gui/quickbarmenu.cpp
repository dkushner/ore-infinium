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

#include "src/quickbarinventory.h"

#include "src/client/client.h"
#include "src/client/gui/gui.h"

#include <src/debug.h>
#include <src/item.h>

#include <Rocket/Core.h>
#include <Rocket/Controls.h>

QuickBarMenu::QuickBarMenu(Client* client, QuickBarInventory* inventory)
    :   m_client(client),
        m_inventory(inventory)
{
    loadDocument();
    selectSlot(0);
}

QuickBarMenu::~QuickBarMenu()
{
}

void QuickBarMenu::ProcessEvent(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    const Rocket::Core::String& type = event.GetType();

    if (id == "0") {
        selectSlot(0);
    } else if (id == "1") {
        selectSlot(1);
    } else if (id == "2") {
        selectSlot(2);
    } else if (id == "3") {
        selectSlot(3);
    } else if (id == "4") {
        selectSlot(4);
    } else if (id == "5") {
        selectSlot(5);
    } else if (id == "6") {
        selectSlot(6);
    } else if (id == "7") {
        selectSlot(7);
    }
}

void QuickBarMenu::loadDocument()
{
    m_menu = GUI::instance()->context()->LoadDocument("../client/gui/assets/quickBarMenu.rml");

    //Rocket::Core::Colourb shit(255, 255, 0, 155);
    //m_menu->SetProperty("background-color", Rocket::Core::Property(shit, Rocket::Core::Property::COLOUR));

//    m_menu->GetElementById("0sub")->SetProperty("background-color", Rocket::Core::Property(shit2, Rocket::Core::Property::COLOUR));
//    m_chat->GetElementById("title")->SetInnerRML("Chat");

//    m_tabSet = dynamic_cast<Rocket::Controls::ElementTabSet*>(m_chat->GetElementById("tabset"));
//    background-image: ../../../../textures/entities.png 0px 0px 22px 22px;
//    background-image: ../../../../textures/entities.png 0px 0px 22px 22px;
//   m_menu->GetElementById("0sub")->SetProperty("background-decorator", Rocket::Core::Property("image", Rocket::Core::Property::STRING));
    m_menu->GetElementById("0sub")->SetAttribute("background-decorator", "image");
//    m_menu->GetElementById("0sub")->SetProperty("background-image", Rocket::Core::Property("../../../../textures/entities.png 0px 0px 22px 22px", Rocket::Core::Property::STRING));
//    m_menu->GetElementById("0sub")->SetProperty("background-image", Rocket::Core::Property("../../../../textures/entities.png 0px 0px 512px 512px", Rocket::Core::Property::KEYWORD));
    m_menu->GetElementById("0sub")->SetAttribute("background-image", "../../../../textures/entities.png 0px 0px 512px 512px");

    m_menu->GetElementById("0")->AddEventListener("click", this);
    m_menu->GetElementById("1")->AddEventListener("click", this);
    m_menu->GetElementById("2")->AddEventListener("click", this);
    m_menu->GetElementById("3")->AddEventListener("click", this);
    m_menu->GetElementById("4")->AddEventListener("click", this);
    m_menu->GetElementById("5")->AddEventListener("click", this);
    m_menu->GetElementById("6")->AddEventListener("click", this);
    m_menu->GetElementById("7")->AddEventListener("click", this);
}

void QuickBarMenu::selectSlot(uint8_t index)
{
    m_inventory->selectSlot(index);

    Rocket::Core::Colourb unselectedColor(0, 0, 255, 255);
    m_menu->GetElementById("0")->SetProperty("background-color", Rocket::Core::Property(unselectedColor, Rocket::Core::Property::COLOUR));
    m_menu->GetElementById("1")->SetProperty("background-color", Rocket::Core::Property(unselectedColor, Rocket::Core::Property::COLOUR));
    m_menu->GetElementById("2")->SetProperty("background-color", Rocket::Core::Property(unselectedColor, Rocket::Core::Property::COLOUR));
    m_menu->GetElementById("3")->SetProperty("background-color", Rocket::Core::Property(unselectedColor, Rocket::Core::Property::COLOUR));
    m_menu->GetElementById("4")->SetProperty("background-color", Rocket::Core::Property(unselectedColor, Rocket::Core::Property::COLOUR));
    m_menu->GetElementById("5")->SetProperty("background-color", Rocket::Core::Property(unselectedColor, Rocket::Core::Property::COLOUR));
    m_menu->GetElementById("6")->SetProperty("background-color", Rocket::Core::Property(unselectedColor, Rocket::Core::Property::COLOUR));
    m_menu->GetElementById("7")->SetProperty("background-color", Rocket::Core::Property(unselectedColor, Rocket::Core::Property::COLOUR));

    Rocket::Core::Colourb selectedColor(0, 255, 0, 255);

    switch (index) {
    case 0:
        m_menu->GetElementById("0")->SetProperty("background-color", Rocket::Core::Property(selectedColor, Rocket::Core::Property::COLOUR));
        break;
    case 1:
        m_menu->GetElementById("1")->SetProperty("background-color", Rocket::Core::Property(selectedColor, Rocket::Core::Property::COLOUR));
        break;
    case 2:
        m_menu->GetElementById("2")->SetProperty("background-color", Rocket::Core::Property(selectedColor, Rocket::Core::Property::COLOUR));
        break;
    case 3:
        m_menu->GetElementById("3")->SetProperty("background-color", Rocket::Core::Property(selectedColor, Rocket::Core::Property::COLOUR));
        break;
    case 4:
        m_menu->GetElementById("4")->SetProperty("background-color", Rocket::Core::Property(selectedColor, Rocket::Core::Property::COLOUR));
        break;
    case 5:
        m_menu->GetElementById("5")->SetProperty("background-color", Rocket::Core::Property(selectedColor, Rocket::Core::Property::COLOUR));
        break;
    case 6:
        m_menu->GetElementById("6")->SetProperty("background-color", Rocket::Core::Property(selectedColor, Rocket::Core::Property::COLOUR));
        break;
    case 7:
        m_menu->GetElementById("7")->SetProperty("background-color", Rocket::Core::Property(selectedColor, Rocket::Core::Property::COLOUR));
        break;
    }

    m_client->sendQuickBarInventorySlotSelectRequest(index);
}

void QuickBarMenu::nextSlot()
{
    //at the right bound
    if (m_inventory->equippedIndex() == m_inventory->maxEquippedSlots() - 1) {
        return;
    }

    selectSlot(m_inventory->equippedIndex() + 1);
}

void QuickBarMenu::previousSlot()
{
    //at the left bound
    if (m_inventory->equippedIndex() == 0) {
        return;
    }

    selectSlot(m_inventory->equippedIndex() - 1);
}

void QuickBarMenu::reloadSlot(uint8_t index)
{
    Debug::log() << "QUICKBAR MENU IS RELOADING SLOT, index: " << index;
    Item* item = m_inventory->item(index);

    std::stringstream ss;
    ss << int(index) << "subcount";

    const std::string id = ss.str();
    ss.str("");

    if (item == nullptr) {
        m_menu->GetElementById(id.c_str())->SetInnerRML("empty");
    } else {
        Debug::log() << "ITEM INFO: name: " << item->name() << " details: " << item->details() << " item stacksize: " << item->stackSize();

        ss << item->stackSize();
        m_menu->GetElementById(id.c_str())->SetInnerRML(ss.str().c_str());
    }
}

void QuickBarMenu::handleEvent(const SDL_Event& event)
{
    switch (event.type) {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_1) {
            selectSlot(0);
        } else if (event.key.keysym.sym == SDLK_2) {
            selectSlot(1);
        } else if (event.key.keysym.sym == SDLK_3) {
            selectSlot(2);
        } else if (event.key.keysym.sym == SDLK_4) {
            selectSlot(3);
        } else if (event.key.keysym.sym == SDLK_5) {
            selectSlot(4);
        } else if (event.key.keysym.sym == SDLK_6) {
            selectSlot(5);
        } else if (event.key.keysym.sym == SDLK_7) {
            selectSlot(6);
        } else if (event.key.keysym.sym == SDLK_8) {
            selectSlot(7);
        }
        break;
    }
}

bool QuickBarMenu::visible()
{
    return m_menu->IsVisible();
}

Rocket::Core::ElementDocument* QuickBarMenu::document()
{
    return m_menu;
}

void QuickBarMenu::show()
{
    m_menu->Show();
}
