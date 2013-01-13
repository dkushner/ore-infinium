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

Entity::Entity(const std::string& frameName, SpriteSheetManager::SpriteSheetType spriteSheetType) :
Sprite(frameName, spriteSheetType)
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

void Entity::update(const float elapsedTime)
{
//    m_velocity.y += GRAVITY;

    glm::vec2 dest = glm::vec2(m_velocity.x() * elapsedTime, m_velocity.y() * elapsedTime);
    dest.x() += position().x();
    dest.y() += position().y();
    //Add the following line to the code with proper variables for width/height.
    //Then you will be able to switch to the newer collsion method.
    //Eigen::Vector2f dim = Eigen::Vector2f(width, height);

    //Newer collision method, add when the line mentioned above is fixed..
    //Texture::setPosition(moveOutsideSolid(position, dest, dim));

    //Remove this and add the above when the definition for dim is fixed.
    if (!World::instance()->isBlockSolid(dest)) {
        Sprite::setPosition(dest);
    }
}

void Entity::setPosition(float x, float y)
{
    Sprite::setPosition(x, y);
}

void Entity::setPosition(const glm::vec2& vect)
{
    Sprite::setPosition(vect);
}

glm::vec2 Entity::moveOutsideSolid(const glm::vec2& firstPosition, const glm::vec2& destPosition, const glm::vec2& dimensions) const
{
    glm::vec2 tempPosition = firstPosition;
    if (checkTileCollision(destPosition, dimensions)) {
        int horDir;
		if (m_velocity.x() > 0) {
			horDir = 1;
		} else if (m_velocity.x() < 0) {
			horDir = -1;
		} else {
			horDir = 0;
		}
        if (horDir != 0) {
            int horMove; 
            for (horMove = int(std::ceil(tempPosition.x()) / Block::blockSize) * Block::blockSize; horMove * horDir <= (tempPosition.x() + m_velocity.x()) * horDir; horMove += Block::blockSize * horDir) {
                glm::vec2 tempDest = tempPosition;
                tempDest.x() = horMove;
                if (checkTileCollision(tempDest, dimensions)) {
                    horMove -= Block::blockSize * horDir;
					if (horDir == 1) {
						horMove += Block::blockSize - (dimensions.x() % Block::blockSize);
					}
                    break;
                }
            }
            if (horMove * horDir > (tempPosition.x() + m_velocity.x()) * horDir) {
                horMove = tempPosition.x() + m_velocity.x();
            }
            tempPosition.x() = horMove;
        }
        int verDir;
		if (m_velocity.y() > 0) {
			verDir = 1;
		} else if (m_velocity.y() < 0) {
			verDir = -1;
		} else {
			verDir = 0;
		}
        if (verDir != 0) {
            int verMove; 
            for (verMove = int(std::ceil(tempPosition.y()) / Block::blockSize) * Block::blockSize; verMove * verDir <= (tempPosition.y() + m_velocity.y()) * verDir; verMove += Block::blockSize * horDir) {
                Eigen::Vector2f tempDest = tempPosition;
                tempDest.y() = verMove;
                if (checkTileCollision(tempDest, dimensions)) {
                    verMove -= Block::blockSize * verDir;
					if (verDir == 1) {
						verMove += Block::blockSize - (dimensions.y() % Block::blockSize);
					}
                    break;
                }
            }
            if (verMove * verDir <= (tempPosition.y() + m_velocity.y()) * verDir) {
                verMove = tempPosition.y() + m_velocity.y();
            }
            tempPosition.y() = verMove;
        }
    } else {
        tempPosition = destPosition;
    }
    return (tempPosition);
}

bool Entity::collidingWithTile(const glm::vec2& destPosition, const glm::vec2& dimensions) const
{
    for (int i = 0; i < dimensions.x() + Block::blockSize; i += Block::blockSize) {
        for (int j = 0; j < dimensions.y() + Block::blockSize; j += Block::blockSize) {
            Eigen::Vector2f tempDest = destPosition;
            tempDest.x() += i;
            tempDest.y() += j;
            if (isBlockSolid(tempDest)) {
                return (true);
            }
        }
    }
    return (false);
}