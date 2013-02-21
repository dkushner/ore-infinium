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

#include "player.h"

#include "src/camera.h"

#include "spritesheetmanager.h"
#include "debug.h"
#include <assert.h>

Player::Player(const std::string& frameName): Entity(frameName, SpriteSheetRenderer::SpriteSheetType::Character)
{

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

void Player::move(int32_t directionX, int32_t directionY)
{
    Entity::setVelocity(directionX * movementSpeed, directionY * movementSpeed);
    Debug::log() << "PLAYER MOVE HAPPENING";
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
