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

#ifndef ITEM_H
#define ITEM_H

#include "entity.h"

#include <vector>
#include <string>

class Item;
class World;

/**
 * Generic representation of an item, like a torch, statue, potion, pickaxe, sword, chest.
 * NOTE: that an Item is more like a stack (not programming stack, but rpg item stack),
 * in the sense that it has a max stack size of say, 40 (it's a per-item basis), which is held in
 * one slot in an inventory. Dropping an item from it drops n items from the stack
 */
class Item : public Entity
{
public:
    Item(const std::string& frameName);
    ~Item();

    enum ItemProperties {
        Placeable = 0,
        Consumable, //potions and such
        Block /// blocks are handled super specially. they have different placement rules, adn they are not rendered as an Entity, but something totally different.
    };

    /*
    /// only one allowed..
    enum EquippableSlots {
       None,
       Hand,
       Legs,
       Chest
    };

    */

    /**
     * NOTE: when an item is in the inventory, it has a state of neither dropped or placed.
     * This is because it is no longer in the world, so it is implied, because the only existence of it, is owned
     * by the inventory.
     */
    enum ItemState {
       Dropped = 0,
       Placed
    };

    //FIXME: unused, would be needed to make it abide by collision/gravity, i'd bet. maybe it can just call the base class (Entity)
    virtual void update(double elapsedTime, World* world);

    /**
     * Typically meaning the left mouse has been pressed.
     * In most cases it means firing a weapon/picking an axe.
     * It's up to the implementation.
     */
    virtual void activatePrimary();

    /**
     * Typically meaning the right mouse has been pressed.
     * In most cases this means placing an item. In other cases it means firing an alternate
     * weapon. It's up to the implementation.
     */
    virtual void activateSecondary();

    /**
     * Drops the requested amount from this item's possible stack, onto the ground. And returns an Item
     * representing the amount dropped with its state set to dropped.
     *
     * Example:
     * If you have 53 torches and @p amount is 2, then it will return an item with 2 torches and 53-2 = 51 torches.
     *
     * Query the amount in the stack via stackSize();
     */
    Item dropStack(uint32_t amount);

    void setStackSize(uint32_t stackSize) { m_stackSize = stackSize; }
    uint32_t stackSize() { return m_stackSize; }

    uint32_t state() { return m_state; }
    void setState(uint32_t state) { m_state = state; }

protected:
    uint32_t m_properties = 0;

private:
    /**
     * Each item subclass needs to set this, the default is 0 and indicates invalid.
     */
    uint32_t m_maximumStackSize = 0;

    /**
     * Indicative of how many items are in this stack, e.g. 32 if there this item represents 32 torches.
     */
    uint32_t m_stackSize = 0;

    uint32_t m_state = 0;
};

#endif