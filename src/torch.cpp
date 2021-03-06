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

#include "torch.h"
#include "debug.h"

Torch::Torch(const glm::vec2& position): Item("torch1Ground1")
{
    Item::setPosition(position);
    Item::setName("Torch Testname");
    Item::setDetails("a motherfucking torch.");
    Item::m_maximumStackSize = 64;
    Item::m_type = Item::ItemType::Torch;
    Item::m_properties = Item::ItemProperties::Placeable;
}

Torch::~Torch()
{

}

Item* Torch::duplicate()
{
    Torch* torch = new Torch(*this);
    Debug::log(Debug::ServerEntityCreationArea) << "duplicating torch, position: " << torch->position().x << " y: " << torch->position().y;

    return torch;
}

void Torch::activatePrimary()
{
}

void Torch::activateSecondary()
{
}
