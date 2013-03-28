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

#ifndef CONTACTLISTENER_H
#define CONTACTLISTENER_H

#include <Box2D/Box2D.h>

class ContactListener : public b2ContactListener
{
public:
    ContactListener();
    virtual ~ContactListener();

    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);

    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

    enum BodyType {
        Invalid = -1,
        Player = 0,
        PlayerFootSensor = 1,
        Block
    };

    struct BodyUserData {
       int type = -1;
       void* data = nullptr;
    };

private:
    void checkBeginContact(BodyUserData* userData);
    void checkEndContact(BodyUserData* userData);
};

#endif
