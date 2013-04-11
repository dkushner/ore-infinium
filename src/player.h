/******************************************************************************
 *   Copyright (C) 2012, 2013 by Shaun Reich <sreich@kde.org>                 *
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

#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "block.h"

#include <SDL2/SDL_events.h>

class Timer;
class Timer;
class QuickBarInventory;
class Camera;
class World;

namespace Entities
{
class Player : public Entity
{
public:
    /**
     * Create a player with the starting frame texture
     */
    Player(const std::string& frameName);

//    void handleEvent(const SDL_Event& event);

    void setName(const std::string& name);
    std::string name() const;

    void setPlayerID(uint32_t id);
    uint32_t playerID() const;

    virtual void createPhysicsBody(World* world, const glm::vec2& position);

    /**
     * Server side only, to asociate and keep track of each player's (clients) mouse position
     */
    void setMousePositionWorldCoords(float x, float y) {
        m_mousePositionWorldCoords = glm::vec2(x, y);
    }
    glm::vec2 mousePositionWorldCoords() {
        return m_mousePositionWorldCoords;
    }

    void setMouseLeftButtonHeld(bool held) {
        m_mouseLeftButtonHeld = held;
    }
    void setMouseRightButtonHeld(bool held) {
        m_mouseRightButtonHeld = held;
    }
    bool mouseLeftButtonHeld() {
        return m_mouseLeftButtonHeld;
    }
    bool mouseRightButtonHeld() {
        return m_mouseRightButtonHeld;
    }

    void setQuickBarInventory(QuickBarInventory* inventory) {
        m_quickBarInventory = inventory;
    }
    QuickBarInventory* quickBarInventory() {
        return m_quickBarInventory;
    }

    bool canAttackPrimary();

    void placeItem();
    bool canPlaceItem();

    /**
     * Accepts player input result, which is a vector from -1 to 1 indicating
     * direction, not magnitude.
     *
     * Only used by server.
     */
    void move(int32_t directionX, int32_t directionY);

    void addJumpContact();
    void removeJumpContact();
    void jump();

public:
    // radius indicating how many pixels out the player can pick blocks
    static constexpr float blockPickingRadius = Block::BLOCK_SIZE * 8.0f;

    //FIXME: MOVE MORE UPSTREAM, halfass attempt at making a unit system, instead of making it pixel based for movement.
    static constexpr float movementUnits = 1.0f / 100.0f;
    //HACK: was 10, 1.0 now for debugging
    static constexpr float movementSpeed = 1.0f * movementUnits;

private:
    uint32_t m_maxHealth = 2500;
    uint32_t m_health = m_maxHealth;

    /**
     * Unique and utilized only by players, is not global or related to generic entity id's
     * Is used to identify the players, for knowing which one the network is talking about,
     * and is also very useful for kicking/banning.
     */
    uint32_t m_playerID;

    /// milliseconds
    const int64_t m_placeableDelay = 500;

    /**
     * NOTE: presently not designed to allow name changing mid-connection. You need to reconnect to do that.
     * (allowing name changes seems likemore of a problem than a solution to anything.)
     */
    std::string m_name;

    /// only utilized by server. as of present, the client stores this in itself
    QuickBarInventory* m_quickBarInventory = nullptr;

    Timer* m_placeableDelayTimer = nullptr;

    /**
     * The mouse position, converted into world coordinates (by the client, and received by server).
     */
    glm::vec2 m_mousePositionWorldCoords = glm::vec2(0.0f, 0.0f);
    bool m_mouseLeftButtonHeld = false;
    bool m_mouseRightButtonHeld = false;

    // if > 0 means something is touching our feet, so we can jump.
    uint32_t m_jumpContacts = 0;

    uint16_t m_ping = 0;

    Timer* m_jumpTimer = nullptr;
    uint32_t m_jumpDelay = 300;
};

}

#endif
