/******************************************************************************
 *   Copyright (C) 2012 by Shaun Reich <sreich@kde.org>                       *
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

#include "entity.h"

#include "world.h"
#include "debug.h"
#include "block.h"

Entity::Entity(const std::string& frameName, SpriteSheetRenderer::SpriteSheetType spriteSheetType)
    : Sprite(frameName, spriteSheetType)
{

}

void Entity::setVelocity(const glm::vec2& velocity)
{
    m_velocity = velocity;
}

void Entity::setVelocity(float x, float y)
{
    const glm::vec2 velocity(x, y);
    m_velocity = velocity;
}

void Entity::update(double elapsedTime, World* world)
{
//    m_velocity.y += GRAVITY;
    if (m_velocity.x > 0.0f || m_velocity.x < 0.0f || m_velocity.y > 0.0f || m_velocity.y < 0.0f) {
        glm::vec2 dest = glm::vec2(m_velocity.x * elapsedTime, m_velocity.y * elapsedTime);
        dest.x += position().x;
        dest.y += position().y;
        //Add the following line to the code with proper variables for width/height.
        //Then you will be able to switch to the newer collsion method.
        //Eigen::Vector2f dim = Eigen::Vector2f(width, height);

        //Newer collision method, add when the line mentioned above is fixed..
        //Texture::setPosition(moveOutsideSolid(position, dest, dim)); FIXME <--- pass world pointer to that

        //Remove this and add the above when the definition for dim is fixed.
//    if (!World::instance()->isBlockSolid(dest)) {

        m_dirtyFlags |= DirtyFlags::PositionDirty;
        Sprite::setPosition(dest);
    }
//   }
}

void Entity::setPosition(float x, float y)
{
    m_dirtyFlags |= DirtyFlags::PositionDirty;
    Sprite::setPosition(x, y);
}

void Entity::setPosition(const glm::vec2& vect)
{
    m_dirtyFlags |= DirtyFlags::PositionDirty;
    Sprite::setPosition(vect);
}

glm::vec2 Entity::moveOutsideSolid(const glm::vec2& firstPosition, const glm::vec2& destPosition, const glm::ivec2& dimensions, World* world) const
{
    glm::vec2 tempPosition = firstPosition;
    if (collidingWithTile(destPosition, dimensions, world)) {
        int horDir;
        if (m_velocity.x > 0) {
            horDir = 1;
        } else if (m_velocity.x < 0) {
            horDir = -1;
        } else {
            horDir = 0;
        }

        if (horDir != 0) {
            int horMove;
            for (horMove = int(std::ceil(tempPosition.x) / Block::BLOCK_SIZE) * Block::BLOCK_SIZE; horMove * horDir <= (tempPosition.x + m_velocity.x) * horDir; horMove += Block::BLOCK_SIZE * horDir) {
                glm::vec2 tempDest = tempPosition;
                tempDest.x = horMove;
                if (collidingWithTile(tempDest, dimensions, world)) {
                    horMove -= Block::BLOCK_SIZE * horDir;
                    if (horDir == 1) {
                        horMove += Block::BLOCK_SIZE - (dimensions.x % Block::BLOCK_SIZE);
                    }
                    break;
                }
            }

            if (horMove * horDir > (tempPosition.x + m_velocity.x) * horDir) {
                horMove = tempPosition.x + m_velocity.x;
            }
            tempPosition.x = horMove;
        }

        int verDir;
        if (m_velocity.y > 0) {
            verDir = 1;
        } else if (m_velocity.y < 0) {
            verDir = -1;
        } else {
            verDir = 0;
        }

        if (verDir != 0) {
            int verMove;
            for (verMove = int(std::ceil(tempPosition.y) / Block::BLOCK_SIZE) * Block::BLOCK_SIZE; verMove * verDir <= (tempPosition.y + m_velocity.y) * verDir; verMove += Block::BLOCK_SIZE * horDir) {
                glm::vec2 tempDest = tempPosition;
                tempDest.y = verMove;

                if (collidingWithTile(tempDest, dimensions, world)) {
                    verMove -= Block::BLOCK_SIZE * verDir;

                    if (verDir == 1) {
                        verMove += Block::BLOCK_SIZE - (dimensions.y % Block::BLOCK_SIZE);
                    }
                    break;
                }
            }

            if (verMove * verDir <= (tempPosition.y + m_velocity.y) * verDir) {
                verMove = tempPosition.y + m_velocity.y;
            }

            tempPosition.y = verMove;
        }
    } else {
        tempPosition = destPosition;
    }

    return tempPosition;
}

bool Entity::collidingWithTile(const glm::vec2& destPosition, const glm::ivec2& dimensions, World* world) const
{
    for (int i = 0; i < dimensions.x + Block::BLOCK_SIZE; i += Block::BLOCK_SIZE) {
        for (int j = 0; j < dimensions.y + Block::BLOCK_SIZE; j += Block::BLOCK_SIZE) {
            glm::vec2 tempDest = destPosition;
            tempDest.x += i;
            tempDest.y += j;
            if (world->isBlockSolid(tempDest)) {
                return true;
            }
        }
    }
    return false;
}

uint32_t Entity::dirtyFlags()
{
    return m_dirtyFlags;
}

void Entity::clearDirtyFlag(uint32_t dirtyFlag)
{
    m_dirtyFlags &= ~dirtyFlag;
}

