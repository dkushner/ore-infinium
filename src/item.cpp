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

#include "item.h"

#include "src/world.h"
#include "src/player.h"

#include "debug.h"

Item::Item(const std::string& frameName) : Entity(frameName, SpriteSheetRenderer::SpriteSheetType::Entity)
{

}

Item::~Item()
{

}

void Item::update(double elapsedTime, World* world)
{
//    Entity::update(elapsedTime, world);
}

void Item::activatePrimary()
{

}

void Item::activateSecondary()
{

}

Item* Item::duplicate()
{
    //unimp, invalid.
    return nullptr;
}

uint32_t Item::dropStack(uint32_t amount)
{
    uint32_t newItemAmount = 0;

    if (amount > m_stackSize) {
        Debug::log() << "item was told to drop a stack amount greater than m_stackSize, ignoring and dropping as much as we can.";

        newItemAmount = m_stackSize;
        m_stackSize = 0;
    } else {
        m_stackSize -= amount;
        newItemAmount = amount;
    }

    return newItemAmount;
}

