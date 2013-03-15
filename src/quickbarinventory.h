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

#ifndef QUICKINVENTORY_H
#define QUICKINVENTORY_H

#include <vector>
#include <string>

class Item;

class QuickBarInventory
{
public:
    QuickBarInventory();
    ~QuickBarInventory();

    /**
     * Sets the Item stored in this slot
     * @p item Ownership is NOT taken on this pointer.
     * @p index 0-based index of the slot and data stored therein
     */
    void setSlot(uint8_t index, Item* item);

    /**
     * Ownership of pointer is caller's responsibility. Will NOT delete.
     * Removes @p index from the item/slot list.
     */
    void removeItem(uint8_t index);

    /**
     * Returns the item at @p index, or nullptr if no item exists there.
     */
    Item* item(uint8_t index);

    /**
     * Only useful for the client, should be called *before* a new item is being set to replace the old one,
     * as the client is a bit different and doesn't hang onto stuff in inventories, as the items get wiped out and
     * re-added in whichever state they are in, if any(e.g. in a chest inventory, quickbar, main player inventory, etc.)
     *
     * NOTE: the server should never need to call this, as the server should just set the pointer at that index to nullptr,
     * and move the item pointer to whereever the new place is., as the client is just acting as a dumb viewer, essentially,
     * but the server needs to keep track of it all.
     */
    void deleteItem(uint8_t index);

    /**
     * Selects/equips the @p index slot
     */
    void selectSlot(uint8_t index) { m_equippedIndex = index; }
    uint8_t equippedIndex() { return m_equippedIndex; }

    uint8_t maxEquippedSlots() { return m_maxEquippedSlots; }

private:
    std::vector<Item*> m_items;

    /// 1-indexed. 1-8, initially/for now.
    uint8_t m_maxEquippedSlots = 8;
    uint8_t m_equippedIndex = 0;
};

#endif
