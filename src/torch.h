/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
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

#ifndef TORCH_H
#define TORCH_H

#include "item.h"

#include <vector>
#include <string>

/**
 */
class Torch : public Item
{
public:
    Torch(const glm::vec2& position);
    ~Torch();

    /**
     * Typically meaning the left mouse has been pressed.
     * In most cases it means firing a weapon/picking an axe.
     * It's up to the implementation.
     */
    virtual void activatePrimary();

    /**
     * Typically meaning the right mouse has been pressed.
     * In most cases this means placing an item. In other cases it means firing an alternate
     * weapon. It's up to the implementation.
     */
    virtual void activateSecondary();

    float radius() { return m_radius; }
    void setRadius(float radius) { m_radius = radius; }

private:
    float m_radius = 300.0f;
};

#endif
