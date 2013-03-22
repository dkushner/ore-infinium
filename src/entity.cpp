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

void Entity::createPhysicsBody(World* world)
{

    //create dynamic body
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    //        bodyDef.position.Set(pixelsToMeters(200), -pixelsToMeters(100));
    bodyDef.position.Set(World::pixelsToMeters(2350.0f), World::pixelsToMeters(1420.f));

    m_body = world->m_box2DWorld->CreateBody(&bodyDef);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(World::pixelsToMeters(50.0f), World::pixelsToMeters(50.0f));

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;

    m_body->CreateFixture(&fixtureDef);
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

        setPosition(dest);
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

uint32_t Entity::dirtyFlags()
{
    return m_dirtyFlags;
}

void Entity::clearDirtyFlag(uint32_t dirtyFlag)
{
    m_dirtyFlags &= ~dirtyFlag;
}

