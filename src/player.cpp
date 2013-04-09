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

#include "player.h"

#include "src/camera.h"
#include "src/quickbarinventory.h"
#include "src/world.h"
#include <src/server/contactlistener.h>

#include "spritesheetmanager.h"
#include "debug.h"
#include "timer.h"
#include <assert.h>

#include <Box2D/Box2D.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>

namespace Entities
{

Player::Player(const std::string& frameName)
    : Entity(frameName, SpriteSheetRenderer::SpriteSheetType::Character)
{
    m_placeableDelayTimer = new Timer();
    m_jumpTimer = new Timer();
    m_sizeMeters = glm::vec2(Block::BLOCK_SIZE * 2, Block::BLOCK_SIZE * 3);
}

void Player::createPhysicsBody(World* world, const glm::vec2& position)
{
    const glm::vec2 size = this->sizeMeters();

    //create dynamic body
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(position.x, position.y);

    m_body = world->box2DWorld()->CreateBody(&bodyDef);
    // sleeping doesn't make sense for a player body.
    m_body->SetSleepingAllowed(false);

    ContactListener::BodyUserData* userData = new ContactListener::BodyUserData();
    userData->type = ContactListener::BodyType::Player;
    userData->data = this;
    m_body->SetUserData(userData);

    b2PolygonShape dynamicBox;
//    dynamicBox.SetAsBox(size.x * 0.5f, size.y * 0.5f);

    int vertCount = 4;
    b2Vec2 vertices[vertCount];
//
//    vertices[0].Set(2.1f, 1.1f); // top left
//    vertices[1].Set(3.5f, 2.5f); // top center
//    vertices[2].Set(4.1f, 3.1f); // top right
//    vertices[3].Set(5.1f, 4.5f); // center left
//    vertices[4].Set(6.1f, 5.5f); // center right
//    vertices[5].Set(7.1f, 6.9f); // bottom left
//    vertices[6].Set(8.2f, 7.1f); // bottom center
//    vertices[7].Set(9.1f, 8.9f);

    //CCW order
//    vertices[0].Set(2.0f, 0.0f); // top center
//    vertices[1].Set(1.0f, 3.1f); // center left
//    vertices[2].Set(2.0f, 2.5f); // bottom center
//    vertices[3].Set(5.1f, 4.5f); // center right

    vertices[0].Set(0,-1);
    vertices[1].Set(1,1);
    vertices[2].Set(0,2);
    vertices[3].Set(-1,1);

//    vertices[4].Set(6.1f, 5.5f); // center right
//    vertices[5].Set(7.1f, 6.9f); // bottom left
//    vertices[6].Set(8.2f, 7.1f); // bottom center
//    vertices[7].Set(9.1f, 8.9f);


    dynamicBox.Set(vertices, vertCount);

    /*
    vertices[0].Set( (-mWidth/2 + 5.0f) / PhysicsManager::mPhysicsScale, (mHeight/2) / PhysicsManager::mPhysicsScale);
    vertices[1].Set( (-mWidth/2) / PhysicsManager::mPhysicsScale, (mHeight/2 - 5.0f) / PhysicsManager::mPhysicsScale);
    vertices[2].Set( (-mWidth/2) / PhysicsManager::mPhysicsScale, (-mHeight/2 + 5.0f) / PhysicsManager::mPhysicsScale);
    vertices[3].Set( (-mWidth/2 + 5.0f) / PhysicsManager::mPhysicsScale, (-mHeight/2) / PhysicsManager::mPhysicsScale);
    vertices[4].Set( (mWidth/2 - 5.0f) / PhysicsManager::mPhysicsScale, (-mHeight/2) / PhysicsManager::mPhysicsScale);
    vertices[5].Set( (mWidth/2) / PhysicsManager::mPhysicsScale, (-mHeight/2 + 5.0f) / PhysicsManager::mPhysicsScale);
    vertices[6].Set( (mWidth/2) / PhysicsManager::mPhysicsScale, (mHeight/2 - 5.0f) / PhysicsManager::mPhysicsScale);
    vertices[7].Set( (mWidth/2 - 5.0f) / PhysicsManager::mPhysicsScale, (mHeight/2) / PhysicsManager::mPhysicsScale);
    */

    // create main body's fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.0f;

    m_body->CreateFixture(&fixtureDef);

    b2PolygonShape footBox;
    b2FixtureDef footSensorFixtureDef;
    footSensorFixtureDef.shape = &footBox;
    footSensorFixtureDef.isSensor = true;

    footBox.SetAsBox(0.3, 0.1, b2Vec2(0.0f, 0.4f), 0.0f);

    b2Fixture* footSensorFixture = m_body->CreateFixture(&footSensorFixtureDef);

    ContactListener::BodyUserData* userDataFoot = new ContactListener::BodyUserData();
    userDataFoot->type = ContactListener::BodyType::PlayerFootSensor;
    userDataFoot->data = this;
    footSensorFixture->SetUserData(userDataFoot);

    m_body->SetFixedRotation(true);
}

void Player::move(int32_t directionX, int32_t directionY)
{
    Entity::setVelocity(directionX * movementSpeed, directionY * movementSpeed);
}

void Player::jump()
{
    if (m_body) {
        if (m_jumpContacts > 0) {
            if (m_jumpTimer->milliseconds() >= m_jumpDelay) {
                glm::vec2 fullVector = glm::vec2(0, -5);
                b2Vec2 currentVelocity = m_body->GetLinearVelocity();

                float velocityChange = fullVector.y;

                float impulse = m_body->GetMass() * velocityChange;

                m_body->ApplyLinearImpulse(b2Vec2(0, impulse), m_body->GetWorldCenter());

                m_jumpTimer->reset();
            }
        }
    }
}

void Player::addJumpContact()
{
    ++m_jumpContacts;
}

void Player::removeJumpContact()
{
    if (m_jumpContacts == 0) {
        return;
    }

    --m_jumpContacts;
}

void Player::setName(const std::string& name)
{
    m_name = name;
}

std::string Player::name() const
{
    return m_name;
}

void Player::setPlayerID(uint32_t id)
{
    m_playerID = id;
}

uint32_t Player::playerID() const
{
    return m_playerID;
}

bool Player::canAttackPrimary()
{

    //FIXME:
    return true;
}

bool Player::canPlaceItem()
{
    if (m_placeableDelayTimer->milliseconds() >= m_placeableDelay) {
        //player has been forced to wait this long before he can place an item again, so now it's okay to do.
        return true;
    }

    return false;
}

void Player::placeItem()
{
    m_placeableDelayTimer->reset();
}

}
