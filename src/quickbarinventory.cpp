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

#include "quickbarinventory.h"

#include "src/item.h"
#include "debug.h"

QuickBarInventory::QuickBarInventory()
{
    m_items.resize(m_maxEquippedSlots);
}

QuickBarInventory::~QuickBarInventory()
{

}

void QuickBarInventory::setSlot(uint8_t index, Item* item)
{
    assert(index < m_items.size());

    m_items.assign(index, item);
}

Item* QuickBarInventory::item(uint8_t index)
{
    assert(index < m_items.size());

    for (int i = 0; i < m_items.size(); ++i) {
        return m_items[i];
    }
}

void QuickBarInventory::removeItem(uint8_t index)
{
    m_items[index] = nullptr;
}
