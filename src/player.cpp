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
}

/*
void Player::handleEvent(const SDL_Event& event)
{
    switch (event.type) {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_RIGHT) {
            m_inputXDirection = 1.f;
        }

        if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_LEFT) {
            m_inputXDirection = -1.f;
        }

        if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN) {
            m_inputYDirection = 1.f;
        }

        if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP) {
            m_inputYDirection = -1.f;
        }
        break;

    case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_RIGHT) {
            m_inputXDirection = 0.f;
        }

        if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_LEFT) {
            m_inputXDirection = 0.f;
        }

        if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN) {
            m_inputYDirection = 0.f;
        }

        if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP) {
            m_inputYDirection = 0.f;
        }
        break;
    }

    Entity::setVelocity(m_inputXDirection * movementSpeed, m_inputYDirection * movementSpeed);
}
*/

void Player::createPhysicsBody(World* world, const glm::vec2& position)
{

    //create dynamic body
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    //        bodyDef.position.Set(pixelsToMeters(200), -pixelsToMeters(100));
    bodyDef.position.Set(World::pixelsToMeters(position.x), World::pixelsToMeters(position.y));

    m_body = world->box2DWorld()->CreateBody(&bodyDef);
    // sleeping doesn't make sense for a player body.
    m_body->SetSleepingAllowed(false);

    ContactListener::BodyUserData* userData = new ContactListener::BodyUserData();
    userData->type = ContactListener::BodyType::Player;
    userData->data = this;
    m_body->SetUserData(userData);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(World::pixelsToMeters(50.0f), World::pixelsToMeters(50.0f));

    // create main body's fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0f;

    m_body->CreateFixture(&fixtureDef);

    b2FixtureDef footSensorFixtureDef;
    footSensorFixtureDef.shape = &dynamicBox;
    footSensorFixtureDef.isSensor = true;

    dynamicBox.SetAsBox(0.5, 0.3, b2Vec2(0, 1), 0);

    b2Fixture* footSensorFixture = m_body->CreateFixture(&footSensorFixtureDef);

    ContactListener::BodyUserData* userDataFoot = new ContactListener::BodyUserData();
    userDataFoot->type = ContactListener::BodyType::PlayerFootSensor;
    userDataFoot->data = this;
    footSensorFixture->SetUserData(userDataFoot);
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
