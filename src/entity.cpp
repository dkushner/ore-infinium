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
#include "server/contactlistener.h"

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>

Entity::Entity(const std::string& frameName, SpriteSheetRenderer::SpriteSheetType spriteSheetType)
    : Sprite(frameName, spriteSheetType)
{

}

Entity::~Entity()
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

void Entity::createPhysicsBody(World* world, const glm::vec2& position)
{
    assert(!m_body);
}

void Entity::update(double elapsedTime, World* world)
{
    //server, physics enabled
    if (m_body) {
        Debug::log(Debug::Area::ServerEntityLogicArea) << "Sprites present position is: x: " << position().x << " y: " << position().y << " SETTING SPRITE POSITION TO X: " << m_body->GetPosition().x << " Y : " << m_body->GetPosition().y;

        Debug::log(Debug::Area::ServerEntityLogicArea) << "Entity update, apply velocity. velocity is x: " << m_velocity.x << " y: " << m_velocity.y;
        //HACK FIXME:
        glm::vec2 position = glm::vec2(m_body->GetPosition().x, m_body->GetPosition().y);

        this->setPosition(position);

        //        Debug::log() << "SETTING SPRITE POSITION TO X: " << m_body->GetPosition().x << " Y : " << m_body->GetPosition().y <<
        //        "value actually is X: " << Sprite::position().x << " Y: " << Sprite::position().y;

        glm::vec2 fullVector = m_velocity * glm::vec2(300, 300);


        b2Vec2 currentVelocity = m_body->GetLinearVelocity();

        b2Vec2 desiredVelocity = b2Vec2(fullVector.x, fullVector.y);
        float velocityChange = desiredVelocity.x - currentVelocity.x;

        Debug::log(Debug::Area::ServerEntityLogicArea) << "Entity update, desired velocity is x: " << desiredVelocity.x << " y: " << desiredVelocity.y << " current velocity is x: " << currentVelocity.x << " y: " << currentVelocity.y;

        float impulse = m_body->GetMass() * velocityChange;

        m_body->ApplyLinearImpulse(b2Vec2(impulse, 0), m_body->GetWorldCenter());
    }
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

uint32_t Entity::dirtyFlags()
{
    return m_dirtyFlags;
}

void Entity::clearDirtyFlag(uint32_t dirtyFlag)
{
    m_dirtyFlags &= ~dirtyFlag;
}

