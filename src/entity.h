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

#ifndef ENTITY_H
#define ENTITY_H

#include "sprite.h"
#include "collisionmap.h"
#include <glm/core/type.hpp>

class b2Body;
class World;
const float GRAVITY = 0.05f;

class Entity : public Sprite
{
public:
    /**
     * Create an entity with the given texture @p texture
     * texture loading is handled automatically via textureManager.
     */
    Entity(const std::string& frameName, SpriteSheetRenderer::SpriteSheetType spriteSheetType);
    ~Entity();

    virtual void update(double elapsedTime, World* world);

    enum DirtyFlags {
        PositionDirty = 1 << 0
    };

    uint32_t dirtyFlags();
    void clearDirtyFlag(uint32_t dirtyFlag);

    void createPhysicsBody(World* world, const glm::vec2& position);
    b2Body* physicsBody();

    /**
     * Use only to reset the entities position to some other place.
     * ALL OTHER CASES USE VELOCITY.
     * Upon calling update, it will move it by that vector, as well as integrate
     * gravity.
     * @see setVelocity
     * @see update
     */
    void setPosition(float x, float y);
    void setPosition(const glm::vec2& vect);

    /**
     * Sets the velocity that this entity should have. It is affected by gravity,
     * and will modify the position automatically when Entity::update() is called.
     */
    void setVelocity(const glm::vec2& velocity);
    void setVelocity(float x, float y);

    void setName(const std::string& name) {
        m_name = name;
    }
    std::string name() {
        return m_name;
    }

    void setDetails(const std::string& details) {
        m_details = details;
    }
    std::string details() {
        return m_details;
    }

protected:
    b2Body* m_body = nullptr;

private:
    glm::vec2 m_velocity = glm::vec2(0, 0);
    uint32_t m_dirtyFlags = 0;

    /// utilized only by collision map for speedy lookups (insertions and removals as well as findings)
    int m_index = -1;

    std::string m_name;
    std::string m_details;


    friend class CollisionMap;
};

#endif
